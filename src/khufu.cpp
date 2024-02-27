// Copyright (c) 2024 Frederic Delhoume
// All rights reserved

#include <signal.h>
#include "mongoose.h"

extern "C" {
#include <tiffio.h>
#include <zlib.h>
#include <jpeglib.h> 
}

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


// TODO: use libjpeg-turbo for maximum performance and support of 1 byte per
// pixel images (gain in bandwidth = 3, speed to evalute)

static int s_debug_level = MG_LL_INFO;
static const char *s_listening_address = "http://0.0.0.0:8000";

static const unsigned int jpeg_quality = 80;

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
  struct mg_connection* c = (struct mg_connection*)cinfo->client_data;
}

boolean MongooseEmpty(j_compress_ptr cinfo) {
  struct mg_connection* c = (struct mg_connection*)cinfo->client_data;
  return true;
}

void MongooseTerm(j_compress_ptr cinfo) {
  struct mg_connection* c = (struct mg_connection*)cinfo->client_data;
}

static void emitJPEG(struct mg_connection *c, int width, int height, int comp, unsigned char* data) {
 // mg_log("Using JPEGLib");
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
// curl -vv http://0.0.0.0:8000/tile/mars_60_40_pyramid/1/34/56 --output tile.jpg


boolean KhufuCheckTile(int position, int numtiles) {
  return (position >= 0) && (position < numtiles);
}


struct mg_str tileapi = mg_str("/tile/*/*/*/*"); // SEP##TILE##SEP##WILD;  


// /tile/<name>/<level>/<x>/<y>
static void cb(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
      struct mg_http_message *hm = (struct mg_http_message *) ev_data;
      struct mg_str uri = hm->uri;
      struct mg_str caps[5];
      char etag[64];
      size_t size = 0;
      time_t mtime = 0;
      struct mg_str *inm = NULL;
      int len = 0;
    
      if (!mg_match(uri, tileapi, caps)) {
          return;
      }
      char filename[128];
      char buffer[8];
      char error[64];

      mg_log("Incoming request %.*s", uri.len, uri.ptr);
      // TODO look for images in a configurable  folder (argv[1] ?)
      mg_snprintf(filename, sizeof(filename), "%.*s.tif", caps[0].len, caps[0].ptr); 
      mg_snprintf(buffer, sizeof(buffer), "%.*s", caps[1].len, caps[1].ptr);
      unsigned int level = atoi(buffer);
      mg_snprintf(buffer, sizeof(buffer), "%.*s", caps[2].len, caps[2].ptr);
      unsigned int column = atoi(buffer);
      mg_snprintf(buffer, sizeof(buffer), "%.*s", caps[3].len, caps[3].ptr);
      unsigned int row = atoi(buffer);
      mg_log("Incoming request: %.*s tile column %d row %d for level %d", caps[0].len, caps[0].ptr, column, row, level);
      int64_t uptime = mg_millis();
      unsigned int nWritten = 0;
      boolean useSTB= false;
      boolean ok = true;

      ok = level >= 0;
      if (ok) {
        TIFF *tifin = TIFFOpen(filename, "r");
        ok = tifin != 0;
        if (ok) {
          tdir_t ndirectories = TIFFNumberOfDirectories(tifin);
          ok = level < ndirectories;
          if (ok) {
            ok = TIFFIsTiled(tifin);
            if (ok) {
              TIFFSetDirectory(tifin, level);
              unsigned int ntiles = TIFFNumberOfTiles(tifin);
              unsigned int tilewidth;
              unsigned int tileheight;
              unsigned int imagewidth;
              unsigned int imageheight;
              uint16_t bits_per_pixel;
              int16_t samples_per_pixel;
              TIFFGetField(tifin, TIFFTAG_IMAGEWIDTH, &imagewidth);
              TIFFGetField(tifin, TIFFTAG_IMAGELENGTH, &imageheight);
              TIFFGetField(tifin, TIFFTAG_TILEWIDTH, &tilewidth);
              TIFFGetField(tifin, TIFFTAG_TILELENGTH, &tileheight);
              TIFFGetFieldDefaulted(tifin, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
              TIFFGetFieldDefaulted(tifin, TIFFTAG_BITSPERSAMPLE, &bits_per_pixel);

              unsigned int numtilesx = imagewidth / tilewidth;
              if (imagewidth % tilewidth)
                ++numtilesx;

              unsigned int numtilesy = imageheight / tileheight;
              if (imageheight % tileheight)
                ++numtilesy;

//              mg_log("directory %d image %dx%d (%dx%d tiles %dx%d)", level, imagewidth, imageheight, numtilesx, numtilesy, tilewidth, tileheight);

              ok = KhufuCheckTile(column, numtilesx);
              if (ok) {
                ok = KhufuCheckTile(row, numtilesy);
                if (ok) {
                  unsigned char *data = 0;
                  if ((bits_per_pixel == 8) && ((samples_per_pixel == 1) || (samples_per_pixel == 3))) {              
                     //  mg_log("Encoded");
                      data = new unsigned char[tilewidth * tileheight * samples_per_pixel];
                      uint32_t tilenum = TIFFComputeTile(tifin, column * tilewidth, row * tileheight, 0, 0);
                      ok = TIFFReadEncodedTile(tifin, tilenum, (uint32_t *)data, tilewidth * tileheight * samples_per_pixel) != -1;
                   } else {
                    //  uncommon formats not handled by libJPEG
                    // mg_log("RGBA");
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
                        // mg_log("using stb: %d %d", bits_per_pixel, samples_per_pixel);
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
                    mg_log("%d bytes, took %d ms", nWritten, elapsed);
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
         TIFFClose(tifin);
      }
      if (!ok) {
        mg_log(error);
        mg_http_reply(c, 404, "",  error);
      }
    }
  }

static void usage(const char *prog) {
  fprintf(stderr,
          "Mongoose v.%s\n"
          "Usage: %s OPTIONS\n"
          "  -l ADDR   - listening address, default: '%s'\n"
          "  -v LEVEL  - debug level, from 0 to 4, default: %d\n",
          MG_VERSION, prog, s_listening_address, s_debug_level);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  char path[MG_PATH_MAX] = ".";
  struct mg_mgr mgr;
  struct mg_connection *c;
  int i;

  // Parse command-line flags
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-l") == 0) {
      s_listening_address = argv[++i];
    } else if (strcmp(argv[i], "-v") == 0) {
      s_debug_level = atoi(argv[++i]);
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
  MG_INFO(("Khufu tile server version : v%s", "0.2"));
  MG_INFO(("Mongoose version : v%s", MG_VERSION));
  MG_INFO(("Listening on     : %s", s_listening_address));
  while (s_signo == 0) mg_mgr_poll(&mgr, 1000);
  mg_mgr_free(&mgr);
  MG_INFO(("Exiting on signal %d", s_signo));
  return 0;
}
