// Copyright (c) 2024 Frederic Delhoume
// All rights reserved

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <signal.h>
#include <time.h>
#define MG_ENABLE_CUSTOM_LOG
#include "mongoose.h"

extern "C" {
#include <jpeglib.h>
#include <tiffio.h>
#include <zlib.h>
}

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "json.hpp"

using namespace nlohmann;

#include "show_template.h"

static int s_debug_level = MG_LL_INFO;
static int s_listening_port = 8000;
static int s_listening_port_external = s_listening_port;
static const char *s_listening_address = "0.0.0.0";
static const char *s_image_folder = ".";
static const char *s_root_folder = ".";

static const unsigned int jpeg_quality = 75;

// Handle interrupts, like Ctrl-C
static int s_signo;
static void signal_handler(int signo) { s_signo = signo; }

// could be directly the connection if no more than one field is necessary
typedef struct khufu_context {
  struct mg_connection *c;
} khufu_context;

void khufu_write(void *context, void *data, int size) {
  khufu_context *kcontext = (khufu_context *)context;
  struct mg_connection *c = kcontext->c;
  mg_send(c, data, size);
}

struct jpeg_compress_struct cinfo;
struct jpeg_error_mgr jerr;
unsigned char *lpRowBuffer[1];
unsigned long mem_size = 0;
unsigned char *mem = 0;

static void emitJPEG(struct mg_connection *c, int width, int height, int comp,
                     int isvswap, unsigned char *data) {
  MG_VERBOSE(("Using JPEGLib"));
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = comp;
  cinfo.in_color_space =
      comp == 4 ? JCS_EXT_RGBX : (comp == 3 ? JCS_RGB : JCS_GRAYSCALE);

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, jpeg_quality, TRUE);
  jpeg_mem_dest(&cinfo, &mem, &mem_size);
  jpeg_start_compress(&cinfo, TRUE);
  /* Write every scanline ... */
  while (cinfo.next_scanline < cinfo.image_height) {
    unsigned int offset =
        isvswap
            ? ((cinfo.image_height - 1 - cinfo.next_scanline) * width * comp)
            : (cinfo.next_scanline * width * comp);
    lpRowBuffer[0] = data + offset;
    jpeg_write_scanlines(&cinfo, lpRowBuffer, 1);
  }

  jpeg_finish_compress(&cinfo);
}
// test with
// curl -vv http://0.0.0.0:8000/tile/mars_60_40_pyramid/1/34/56 -o tile.jpg

bool KhufuCheckTile(int position, int numtiles) {
  return (position >= 0) && (position < numtiles);
}

struct mg_str tileapi = mg_str("/tile/*/*/*/*");
struct mg_str infoapi = mg_str("/info");
struct mg_str showapi = mg_str("/show/*");

struct TIFFInfo {
  TIFF *tifin = nullptr;
  int num_directories = -1;
  int current_directory = -1;
  bool tiled = false;
  uLong signature = 0;
};

static TIFFInfo cache;
static char error[256];

void cbError(struct mg_str &uri) {
  MG_ERROR(("%.*s -=> %s", uri.len, uri.buf, error));
}

json j;
// updates internal json with info on server tiff folder images
void buildList() {
  j.clear();
  char buf[256] = "";
  while (mg_fs_ls(&mg_fs_posix, s_image_folder, buf, sizeof(buf))) {
    size_t buflen = strlen(buf);
    if (buflen <= 4)
      continue;
    mg_str extension = mg_str_n(buf + buflen - 4, 4);
    mg_str tifext = mg_str(".tif");
    if (mg_strcasecmp(extension, tifext) == 0) {
      char filename[256];
      char id[256];
      memcpy(id, buf, buflen - 4);
      id[buflen - 4] = 0;
      mg_snprintf(filename, sizeof(filename), "%s/%s", s_image_folder, buf);

      TIFFOpenOptions *opts = TIFFOpenOptionsAlloc();
      TIFF *tifin = TIFFOpenExt(filename, "r", opts);
      TIFFOpenOptionsFree(opts);
      if (tifin == nullptr) {
        mg_snprintf(error, sizeof(error), "could not open %s", filename);
        continue;
      }
      bool istiled = TIFFIsTiled(tifin);
      json imgj;
      int ndirs = TIFFNumberOfDirectories(tifin);
      imgj["nlevels"] = ndirs;
      imgj["filename"] = buf;
      imgj["tiled"] = (bool)istiled;
      uint16_t bits_per_sample;
      int16_t samples_per_pixel;
      TIFFGetFieldDefaulted(tifin, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);

      TIFFGetFieldDefaulted(tifin, TIFFTAG_BITSPERSAMPLE, &bits_per_sample);
      imgj["bitspersample"] = bits_per_sample;
      imgj["samplesperpixel"] = samples_per_pixel;
      if (istiled) {
        unsigned int tilewidth;
        unsigned int tileheight;
        TIFFGetField(tifin, TIFFTAG_TILEWIDTH, &tilewidth);
        TIFFGetField(tifin, TIFFTAG_TILELENGTH, &tileheight);
        imgj["tilewidth"] = tilewidth;
        imgj["tileheight"] = tileheight;
        char rbuf[100];
        mg_snprintf(rbuf, sizeof(rbuf), "http://localhost:%d/show/%s",
                    s_listening_port_external, id);
        imgj["link"] = rbuf;
      }
      for (int d = 0; d < ndirs; ++d) {
        TIFFSetDirectory(tifin, d);
        //        fprintf(stderr, "info for %d\n", d);
        unsigned int imagewidth;
        unsigned int imageheight;

        TIFFGetField(tifin, TIFFTAG_IMAGEWIDTH, &imagewidth);
        TIFFGetField(tifin, TIFFTAG_IMAGELENGTH, &imageheight);
        imgj["levels"][d]["index"] = d;
        imgj["levels"][d]["width"] = imagewidth;
        imgj["levels"][d]["height"] = imageheight;

        unsigned int filetype;
        TIFFGetFieldDefaulted(tifin, TIFFTAG_SUBFILETYPE, &filetype);
        imgj["levels"][d]["full"] =
            filetype & FILETYPE_REDUCEDIMAGE ? false : true;
      }
      TIFFClose(tifin);
      j[id] = imgj;
      // fprintf(stderr, "Done buildlist\n%s %s\n", id, j.dump().c_str());
    }
  }
}

// main url parse
static void cb(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_ERROR) {
    printf("Error: %s", (char *)ev_data);
  } else if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    struct mg_str uri = hm->uri;
    struct mg_str caps[5];
    uint64_t starttime = mg_millis();
    if (mg_match(uri, infoapi, caps)) {
      buildList();
      auto cpp_string = j.dump();
      mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s\n",
                    cpp_string.c_str());
    } else if (mg_match(uri, showapi, caps)) {
      buildList();
      char id[128];
      char buffer[2048];
      mg_snprintf(id, sizeof(id), "%.*s", caps[0].len, caps[0].buf);
      if (j.contains(id)) {
        if (j[id].contains("tiled") && j[id]["tiled"] == true) {
          int directories = j[id]["nlevels"];
          // find the one that is not reduced image
          for (int d = 0; d < directories; ++d) {
            if (j[id]["levels"][d]["full"] == true) {
              int tilesize = j[id]["tileheight"];
              int imagewidth = j[id]["levels"][d]["width"].get<int>();
              int imageheight = j[id]["levels"][d]["height"].get<int>();
              int maxdimension = imagewidth;
              if (imageheight > imagewidth) {
                maxdimension = imageheight;
              }
              int maxlevel = ceil(log(maxdimension) / log(2));
              int minlevel = maxlevel - directories + 1;
              int dirfull = j[id]["levels"][d]["index"].get<int>();
              char *html_template = (char *)show_template;
              mg_snprintf(buffer, sizeof(buffer), html_template,
                          s_listening_port, dirfull, id, imagewidth,
                          imageheight, tilesize, minlevel, maxlevel);
              mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s\n",
                            buffer);
              return;
            }
          }
        } else {
          mg_snprintf(error, sizeof(error), "%s is not tiled", id);
          cbError(uri);
          return;
        }
      } else {
        mg_snprintf(error, sizeof(error), "%s is not available", id);
        cbError(uri);
        return;
      }
    } else if (mg_match(uri, tileapi, caps)) {
      char id[256];
      char filename[256];
      char buffer[8];
      mg_snprintf(id, sizeof(id), "%.*s", caps[0].len, caps[0].buf);
      mg_snprintf(filename, sizeof(filename), "%s/%s.tif", s_image_folder, id);
      mg_snprintf(buffer, sizeof(buffer), "%.*s", caps[1].len, caps[1].buf);
      int level = atoi(buffer);
      mg_snprintf(buffer, sizeof(buffer), "%.*s", caps[2].len, caps[2].buf);
      int column = atoi(buffer);
      mg_snprintf(buffer, sizeof(buffer), "%.*s", caps[3].len, caps[3].buf);
      int row = atoi(buffer);
      bool useSTB = false;
      bool ok;

      if (level < 0 || column < 0 || row < 0) {
        mg_snprintf(error, sizeof(error), "Wrong format");
        cbError(uri);
        return;
      }
      MG_INFO(("tile request for %s: directory %d row %d column %d", id, level,
               row, column));
      uLong signature = adler32(0, (const Bytef *)caps[0].buf, caps[0].len);
      TIFF *tifin;
      if (signature != cache.signature) {
        TIFFClose(cache.tifin);

        TIFFOpenOptions *opts = TIFFOpenOptionsAlloc();
        tifin = TIFFOpenExt(filename, "r", opts);
        TIFFOpenOptionsFree(opts);
        if (tifin == nullptr) {
          mg_snprintf(error, sizeof(error), "could not open %s", filename);
          cbError(uri);
          return;
        }
        cache.tifin = tifin;
        cache.num_directories = TIFFNumberOfDirectories(tifin);
        cache.current_directory = 0;
        cache.tiled = TIFFIsTiled(tifin);
        cache.signature = signature;
        if (cache.tiled == false) {
          mg_snprintf(error, sizeof(error), "directory %d is not tiled", level);
          cbError(uri);
          return;
        }
      }
      if (level >= cache.num_directories) {
        mg_snprintf(error, sizeof(error), "directory number %d exceed limit %d",
                    level, cache.num_directories - 1);
        cbError(uri);
        return;
      }
      if (cache.current_directory != level) {
        TIFFSetDirectory(cache.tifin, level);
        cache.current_directory = level;
        // cache.tiled = TIFFIsTiled(tifin);
      }
      if (cache.tiled == false) {
        mg_snprintf(error, sizeof(error), "directory %d is not tiled", level);
        cbError(uri);
        return;
      }
      tifin = cache.tifin;
      unsigned int tilewidth;
      unsigned int tileheight;
      unsigned int imagewidth;
      unsigned int imageheight;
      uint16_t bits_per_sample;
      int16_t samples_per_pixel;
      TIFFGetField(tifin, TIFFTAG_IMAGEWIDTH, &imagewidth);
      TIFFGetField(tifin, TIFFTAG_IMAGELENGTH, &imageheight);
      TIFFGetField(tifin, TIFFTAG_TILEWIDTH, &tilewidth);
      TIFFGetField(tifin, TIFFTAG_TILELENGTH, &tileheight);
      TIFFGetFieldDefaulted(tifin, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
      TIFFGetFieldDefaulted(tifin, TIFFTAG_BITSPERSAMPLE, &bits_per_sample);

      unsigned int numtilesx = imagewidth / tilewidth;
      if (imagewidth % tilewidth)
        ++numtilesx;

      unsigned int numtilesy = imageheight / tileheight;
      if (imageheight % tileheight)
        ++numtilesy;

      if (KhufuCheckTile(column, numtilesx) == false) {
        mg_snprintf(error, sizeof(error), "bad column number %d, max is %d",
                    column, numtilesx - 1);
        cbError(uri);
        return;
      }
      if (KhufuCheckTile(row, numtilesy) == false) {
        mg_snprintf(error, sizeof(error), "bad row number %d, max is %d", row,
                    numtilesy - 1);
        cbError(uri);
        return;
      }

      unsigned char *data = nullptr;
      int vswap = 0;
      if (bits_per_sample == 8 && samples_per_pixel == 1) {
        MG_VERBOSE((" bps %d spp %d", bits_per_sample, samples_per_pixel));
        data =
            (unsigned char *)malloc(tilewidth * tileheight * samples_per_pixel);
        uint32_t tilenum =
            TIFFComputeTile(tifin, column * tilewidth, row * tileheight, 0, 0);
        ok = TIFFReadEncodedTile(tifin, tilenum, (uint32_t *)data,
                                 tilewidth * tileheight * samples_per_pixel) !=
             -1;
      } else {
        //  all other formats
        MG_VERBOSE(
            ("ReadRGBATile bps %d spp %d", bits_per_sample, samples_per_pixel));
        samples_per_pixel = 4;
        data =
            (unsigned char *)malloc(tilewidth * tileheight * samples_per_pixel);
        ok = TIFFReadRGBATile(tifin, column * tilewidth, row * tileheight,
                              (uint32_t *)data) == 1;

        vswap = 1;
      }
      if (!ok) {
        mg_snprintf(error, sizeof(error), "could not read tile");
        cbError(uri);
      }
      if (useSTB == false) {
        emitJPEG(c, tilewidth, tileheight, samples_per_pixel, vswap, data);

        mg_printf(c,
                  "HTTP/1.1 200 OK\r\n"
                  "Content-Type: image/jpeg;\r\n"
                  "Access-Control-Allow-Origin:*\r\n"
                  "Referer:no-referer\r\n"
                  "Content-Length: %d\r\n\r\n",
                  mem_size);
        mg_send(c, mem, mem_size);
        c->is_resp = 0;
        MG_INFO(("     served %d bytes in %d ms", (int)mem_size,
                 (mg_millis() - starttime)));
        return;
      } else {
        // TIFFReadRGBATile returns upside-down data...
        uint32_t *top = (uint32_t *)data;
        uint32_t *bottom =
            (uint32_t *)(data + (tileheight - 1) * tilewidth * 4);
        for (unsigned int yy = 0; yy < tileheight / 2; ++yy) {
          for (unsigned int xx = 0; xx < tilewidth; ++xx) {
            uint32_t temp = top[xx];
            top[xx] = bottom[xx];
            bottom[xx] = temp;
          }
          top += tilewidth;
          bottom -= tilewidth;
        }
        mg_printf(c,
                  "HTTP/1.1 200 OK\r\n"
                  "Content-Type: image/jpeg; "
                  "Access-Control-Allow-Origin:*\r\n"
                  "Referer:no-referer\r\n"
                  "Content-Length:         \r\n\r\n"); // placeholder
        int off = c->send.len;
        khufu_context context = {c};
        stbi_write_jpg_to_func(khufu_write, &context, tilewidth, tileheight, 4,
                               data, jpeg_quality);
        // fill placeholder for size
        char tmp[10];
        size_t n = mg_snprintf(tmp, sizeof(tmp), "%lu",
                               (unsigned long)(c->send.len - off));
        if (n > sizeof(tmp))
          n = 0;
        memcpy(c->send.buf + off - 12, tmp, n); // Set content length
        c->is_resp = 0;                         // Mark response end
      }
      // free(data);
    } else {
      // normal web server but internal path for openseadragon
      struct mg_http_serve_opts optsroot = {
        .root_dir = s_root_folder};
        struct mg_http_serve_opts optsapp = {
          .root_dir = "/app"};
         struct mg_http_serve_opts* opts;
          if (mg_match(uri, mg_str("/app/*"), caps)) {
           opts = &optsapp;
          uri = mg_str_n(caps[0].buf, caps[0].len);
          } else {
           opts = &optsroot;
          }
      struct mg_http_message *hm = (struct mg_http_message *)ev_data;
      char uribuf[256];
      mg_snprintf(uribuf, sizeof(uribuf), "%.*s", hm->uri.len, hm->uri.buf);    
      mg_http_serve_file(c, hm, uribuf, opts);
    }
  }
}

static void usage(const char *prog) {
  fprintf(stdout,
          "Khufu tile server version : v%s\n"
          "Usage: %s OPTIONS\n"
          "  -d FOLDER - web folder, default: .\n"
          "  -f FOLDER - folder with TIFF images, default: .\n"
          "  -h [ADDR]   - listening address, default: '%s'\n"
          "  -p [PORT]   - listening port, default: '%d'\n"
          "  -v LEVEL  - log level, one of NONE|ERROR(default)|INFO|DEBUG|VERBOSE\n",
          KHUFU_VERSION, prog, s_listening_address, s_listening_port);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  struct mg_mgr mgr;
  struct mg_connection *c;
  int i;
  char *debug_level = NULL;

  // Parse command-line flags
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-d") == 0) {
      s_root_folder = argv[++i];
    } else if (strcmp(argv[i], "-") == 0) {
      s_listening_address = argv[++i];
    } else if (strcmp(argv[i], "-p") == 0) {
      s_listening_port = atoi(argv[++i]);
    } else if (strcmp(argv[i], "-v") == 0) {
      debug_level = argv[++i];
    } else if (strcmp(argv[i], "-f") == 0) {
      s_image_folder = argv[++i];
    } else {
      usage(argv[0]);
    }
    // convert debug level to Mongoose log level
    // TODO: simple hash
    if (debug_level == NULL) {
      s_debug_level = MG_LL_ERROR;
    } else {
      if (strcmp(debug_level, "NONE") == 0) {
        s_debug_level = MG_LL_NONE;
      } else if (strcmp(debug_level, "ERROR") == 0) {
        s_debug_level = MG_LL_ERROR;
      } else if (strcmp(debug_level, "INFO") == 0) {
        s_debug_level = MG_LL_INFO;
      } else if (strcmp(debug_level, "DEBUG") == 0) {
        s_debug_level = MG_LL_DEBUG;
      } else if (strcmp(debug_level, "VERBOSE") == 0) {
        s_debug_level = MG_LL_VERBOSE;
      } else {
        fprintf(stderr, "Unknown log level: %s\n", debug_level);
        usage(argv[0]);
      }
    }
    const char *external_port = getenv("EXTERNALPORT");
    if (external_port) {
      s_listening_port_external = atoi(external_port);
    } else {
      s_listening_port_external = s_listening_port;
    }
  }

  //  TIFFSetErrorHandler(NULL);
  //   TIFFSetErrorHandlerExt(NULL);
  TIFFSetWarningHandler(NULL);
  TIFFSetWarningHandlerExt(NULL);

  // Initialise stuff
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  mg_mgr_init(&mgr);
  mg_log_set(s_debug_level);
  char url[100];
  mg_snprintf(url, sizeof(url), "http://%s:%d", s_listening_address,
              s_listening_port);
  if ((c = mg_http_listen(&mgr, url, cb, &mgr)) == NULL) {
    MG_ERROR(("Cannot listen on %s", url));
    exit(EXIT_FAILURE);
  }

  // Start infinite event loop
  MG_INFO(("Khufu tile server version : v%s", KHUFU_VERSION));
  MG_INFO(("Mongoose version          : v%s", MG_VERSION));
  MG_INFO(("Listening on              : %s", url));
  MG_INFO(("Serving http from folder : %s", s_root_folder));
  MG_INFO(("Serving tiles from folder : %s", s_image_folder));
  while (s_signo == 0)
    mg_mgr_poll(&mgr, 1000);
  TIFFClose(cache.tifin);
  mg_mgr_free(&mgr);
  MG_INFO(("Exiting on signal %d", s_signo));
  return 0;
}