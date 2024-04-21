// Copyright (c) 2024 Frederic Delhoume
// All rights reserved

#define KHUFU_VERSION "0.3"

#include <signal.h>
#include "mongoose.h"

extern "C" {
#include <tiffio.h>
#include <zlib.h>
#include <jpeglib.h> 
}

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


static int s_debug_level = MG_LL_INFO;
static const char *s_listening_address = "http://0.0.0.0:8000";
static const char* s_image_folder = ".";

static const unsigned int jpeg_quality = 75;

// Handle interrupts, like Ctrl-C
static int s_signo;
static void signal_handler(int signo) {
  s_signo = signo;
}

 // could be directly the connection if no more than one field is necessary
typedef struct khufu_context {
  struct mg_connection *c;
} khufu_context;

void khufu_write(void *context, void *data, int size) {
  khufu_context* kcontext = (khufu_context*)context;
  struct mg_connection *c= kcontext->c;
  mg_send(c, data, size);
}

void MongooseInit(j_compress_ptr cinfo) {
  //struct mg_connection* c = (struct mg_connection*)cinfo->client_data;
}

boolean MongooseEmpty(j_compress_ptr cinfo) {
 // struct mg_connection* c = (struct mg_connection*)cinfo->client_data;
  return true;
}

void MongooseTerm(j_compress_ptr cinfo) {
 // struct mg_connection* c = (struct mg_connection*)cinfo->client_data;
}

static void emitJPEG(struct mg_connection *c, int width, int height, int comp, unsigned char* data) {
 MG_VERBOSE(("Using JPEGLib"));
  struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr err;
  unsigned char* lpRowBuffer[1];
  cinfo.err = jpeg_std_error(&err);
	jpeg_create_compress(&cinfo);

	//jpeg_stdio_dest(&info, fHandle);

  unsigned char *mem = 0;
  unsigned long mem_size = 0;

  jpeg_mem_dest(&cinfo, &mem, &mem_size);

/*
  struct jpeg_destination_mgr khufu;
  cinfo.dest = &khufu;
  khufu.init_destination = MongooseInit;
  khufu.empty_output_buffer = MongooseEmpty;
  khufu.term_destination = MongooseTerm;
  cinfo.dest = &khufu;
  cinfo.client_data = (void*)c;
  */

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = comp;
	cinfo.in_color_space = comp == 3 ? JCS_RGB : JCS_GRAYSCALE;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, jpeg_quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	/* Write every scanline ... */
	while(cinfo.next_scanline < cinfo.image_height) {
		lpRowBuffer[0] = data + cinfo.next_scanline * width * comp;
		jpeg_write_scanlines(&cinfo, lpRowBuffer, 1);
	}
	jpeg_finish_compress(&cinfo);
  mg_send(c, mem, mem_size);
	jpeg_destroy_compress(&cinfo);
  free((void*)mem);
 }
// test with
// curl -vv http://0.0.0.0:8000/tile/mars_60_40_pyramid/1/34/56 -o tile.jpg


boolean KhufuCheckTile(int position, int numtiles) {
  return (position >= 0) && (position < numtiles);
}


struct mg_str tileapi = mg_str("/tile/*/*/*/*"); // SEP##TILE##SEP##WILD;  

// cache
static uLong adler_signature = 0;
static TIFF* tifin = 0;

// /tile/<name>/<level>/<x>/<y>
static void cb(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
      struct mg_http_message *hm = (struct mg_http_message *) ev_data;
      struct mg_str uri = hm->uri;
      struct mg_str caps[5];
    
      if (!mg_match(uri, tileapi, caps)) {
          return;
      }
      char filename[128];
      char buffer[8];
      char error[256];

      mg_snprintf(filename, sizeof(filename), "%s/%.*s.tif", s_image_folder, caps[0].len, caps[0].ptr); 
      mg_snprintf(buffer, sizeof(buffer), "%.*s", caps[1].len, caps[1].ptr);
      unsigned int level = atoi(buffer);
      mg_snprintf(buffer, sizeof(buffer), "%.*s", caps[2].len, caps[2].ptr);
      unsigned int column = atoi(buffer);
      mg_snprintf(buffer, sizeof(buffer), "%.*s", caps[3].len, caps[3].ptr);
      unsigned int row = atoi(buffer);
 //     mg_log("Incoming request: %.*s tile column %d row %d for level %d", caps[0].len, caps[0].ptr, column, row, level);
      int64_t uptime = mg_millis();
      unsigned int nWritten = 0;
      boolean useSTB= false;
      boolean ok = true;

      ok = level >= 0;
      if (ok) {
        uLong new_signature = adler32(0, (const Bytef*)caps[0].ptr, caps[0].len);
        if (new_signature != adler_signature) {
          TIFFClose(tifin);
          TIFFOpenOptions *opts = TIFFOpenOptionsAlloc();
          tifin = TIFFOpenExt(filename, "r", opts);
          TIFFOpenOptionsFree(opts);
          adler_signature = new_signature;
        } else {
          MG_VERBOSE(("using cached TIFF"));
        }
        ok = tifin != 0;
        if (ok) {
          tdir_t ndirectories = TIFFNumberOfDirectories(tifin);
          ok = level < ndirectories;
          if (ok) {
            ok = TIFFIsTiled(tifin);
            if (ok) {
              TIFFSetDirectory(tifin, level);
//              unsigned int ntiles = TIFFNumberOfTiles(tifin);
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

              MG_INFO(("directory %d image %dx%d (%dx%d tiles %dx%d)", level, imagewidth, imageheight, numtilesx, numtilesy, tilewidth, tileheight));

              ok = KhufuCheckTile(column, numtilesx);
              if (ok) {
                ok = KhufuCheckTile(row, numtilesy);
                if (ok) {
                  unsigned char *data = 0;
                  if ((useSTB == false) && (bits_per_sample == 8) && ((samples_per_pixel == 1) || (samples_per_pixel == 3))) {              
                       MG_VERBOSE(("ReadEncodedTile bps %d spp %d", bits_per_sample, samples_per_pixel));
                      data = new unsigned char[tilewidth * tileheight * samples_per_pixel];
                      uint32_t tilenum = TIFFComputeTile(tifin, column * tilewidth, row * tileheight, 0, 0);
                      ok = TIFFReadEncodedTile(tifin, tilenum, (uint32_t *)data, tilewidth * tileheight * samples_per_pixel) != -1;
                   } else {
                    //  uncommon formats not handled by libJPEG
                     MG_VERBOSE(("ReadRGBATile bps %d spp %d", bits_per_sample, samples_per_pixel));
                      data = new unsigned char[tilewidth * tileheight * 4];
                      ok = TIFFReadRGBATile(tifin, column * tilewidth, row * tileheight, (uint32_t*)data) == 1;
                      useSTB = true;
                   }
                  if (ok) {
                    mg_printf(c, "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg; charset=utf-8\r\nContent-Length:         \r\n\r\n"); // placeholder
                    int off = c->send.len;  // Start of body
                    if (useSTB == false) { // fully optimized path, up to 3 times faster
                      emitJPEG(c, tilewidth, tileheight, samples_per_pixel, data);
                    } else {  // fallback
                        // TIFFReadRGBATile returns upside-down data...
                        uint32_t* top = (uint32_t*)data;
                        uint32_t* bottom = (uint32_t*)(data + (tileheight - 1) * tilewidth * 4);
                        for (unsigned int yy = 0;  yy < tileheight / 2; ++yy) {
                          for (unsigned int xx = 0; xx < tilewidth; ++xx) {
                            uint32_t temp = top[xx];
                            top[xx] = bottom[xx];
                            bottom[xx] = temp;
                          }
                          top += tilewidth;
                          bottom -= tilewidth;
                        }
                        khufu_context context = {c};
                        stbi_write_jpg_to_func(khufu_write, &context, tilewidth, tileheight, 4, data, jpeg_quality);
                    }
                    // fill placeholder for size
                    char tmp[10];
                    size_t n = mg_snprintf(tmp, sizeof(tmp), "%lu", (unsigned long)(c->send.len - off));
                    nWritten = c->send.len - off;
                    if (n > sizeof(tmp))
                      n = 0;
                    memcpy(c->send.buf + off - 12, tmp, n); // Set content length
                    c->is_resp = 0;                         // Mark response end
                    int elapsed = mg_millis() - uptime;
                    MG_INFO(("Incoming request %.*s -=> %d bytes, took %d ms", uri.len, uri.ptr, nWritten, elapsed));
                  } else {
                    mg_snprintf(error, sizeof(error), "could not read tile");
                  }
                  if (data) delete[] data;
                } else {
                    mg_snprintf(error, sizeof(error), "bad row number %d, max is %d", row, numtilesy - 1);
                }
              } else {
                mg_snprintf(error, sizeof(error), "bad column number %d, max is %d", column, numtilesx - 1);
              }
            } else {
              mg_snprintf(error, sizeof(error), "image is not tiled");
            }
          } else {
            mg_snprintf(error, sizeof(error), "directory number %d exceed limit %d", level, ndirectories - 1); 
          }
        } else {
          mg_snprintf(error, sizeof(error), "could not open %s", filename);
        }
      }
      if (!ok) {
        MG_ERROR(("Incoming request %.*s -=> %s", uri.len, uri.ptr, error));
        struct mg_http_serve_opts opts = { .mime_types = "jpg=image/jpg" };
        mg_http_serve_file(c, hm, "default_tile.jpg", &opts);
        //mg_http_reply(c, 404, "",  error);
      }
    }
  }

static void usage(const char *prog) {
  fprintf(stderr,
         "Khufu tile server version : v%ss\n"
          "Usage: %s OPTIONS\n"
          "  -f FOLDER - folder with images, default: .\n"
          "  -l ADDR   - listening address, default: '%s'\n"
          "  -v LEVEL  - debug level, from 0 to 4, default: %d\n",
          KHUFU_VERSION, prog, s_listening_address, s_debug_level);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  struct mg_mgr mgr;
  struct mg_connection *c;
  int i;

  // Parse command-line flags
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-l") == 0) {
      s_listening_address = argv[++i];
    } else if (strcmp(argv[i], "-v") == 0) {
      s_debug_level = atoi(argv[++i]);
    } else if (strcmp(argv[i], "-f") == 0) {
      s_image_folder = argv[++i];
    } else {
      usage(argv[0]);
    }
  }

  //  TIFFSetErrorHandler(NULL);
  //   TIFFSetErrorHandlerExt(NULL);
 TIFFSetWarningHandler(NULL);
 TIFFSetWarningHandlerExt(NULL);

  // Initialise stuff
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  mg_log_set(s_debug_level);
  mg_mgr_init(&mgr);
  if ((c = mg_http_listen(&mgr, s_listening_address, cb, &mgr)) == NULL) {
    MG_ERROR(("Cannot listen on %s. Use http://ADDR:PORT or :PORT", s_listening_address));
    exit(EXIT_FAILURE);
  }

  // Start infinite event loop
  MG_INFO(("Khufu tile server version : v%s", KHUFU_VERSION));
  MG_INFO(("Mongoose version          : v%s", MG_VERSION));
  MG_INFO(("Listening on              : %s", s_listening_address));
  MG_INFO(("Serving tiles from folder : %s", s_image_folder));
   while (s_signo == 0) mg_mgr_poll(&mgr, 1000);
  TIFFClose(tifin);
  mg_mgr_free(&mgr);
  MG_INFO(("Exiting on signal %d", s_signo));
  return 0;
}
