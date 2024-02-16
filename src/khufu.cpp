// Copyright (c) 2024 Frederic Delhoume
// All rights reserved

#include <signal.h>
#include "mongoose.h"

extern "C" {
#include <tiffio.h>
#include <zlib.h>
}

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


static int s_debug_level = MG_LL_INFO;

static const char *s_listening_address = "http://0.0.0.0:8000";
static const char *s_ssi_pattern = "#.html";


// Handle interrupts, like Ctrl-C
static int s_signo;
static void signal_handler(int signo) {
  s_signo = signo;
}

struct mg_str tileapi = mg_str("/tile/*/*/*/*"); // SEP##TILE##SEP##WILD);  
 
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

      // TODO look for images in a configurable  folder
      // TODO temporary folder and file
      mg_snprintf(filename, sizeof(filename), "%.*s.tif", caps[0].len, caps[0].ptr); 
      mg_snprintf(buffer, sizeof(buffer), "%.*s", caps[1].len, caps[1].ptr);
      unsigned int level = atoi(buffer);
      mg_snprintf(buffer, sizeof(buffer), "%.*s", caps[2].len, caps[2].ptr);
      unsigned int column = atoi(buffer);
      mg_snprintf(buffer, sizeof(buffer), "%.*s", caps[3].len, caps[3].ptr);
      unsigned int row = atoi(buffer);

      mg_log("Request : %.*s",  uri.len, uri.ptr);
      mg_log("Loading : %s (directory: %d column: %d row: %d", filename, level, column, row);
      TIFF* tifin = TIFFOpen(filename, "r");
      if (tifin && TIFFIsTiled(tifin)) {
          TIFFSetDirectory(tifin, level);
          unsigned int tilewidth;              
          unsigned int tileheight;
          uint16_t bits_per_pixel;
          uint16_t samples_per_pixel;
          TIFFGetField(tifin, TIFFTAG_TILEWIDTH, &tilewidth);             
          TIFFGetField(tifin, TIFFTAG_TILELENGTH, &tileheight);              
          TIFFGetFieldDefaulted(tifin, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);              
          TIFFGetFieldDefaulted(tifin, TIFFTAG_BITSPERSAMPLE, &bits_per_pixel);
          unsigned char* data = new unsigned char[tilewidth * tileheight * samples_per_pixel];
          //          TIFFReadRGBATile(tifin, column * tilewidth, row * tileheight, (uint32_t*)data);  
         uint32_t tilenum = TIFFComputeTile(tifin, column * tilewidth, row * tileheight, 0, 0); 
          TIFFReadEncodedTile(tifin, tilenum, (uint32_t*)data, tilewidth * tileheight * samples_per_pixel);
          TIFFClose(tifin);

          char tempfile[128] = "/tmp/tile.jpg";
          mg_snprintf(tempfile, sizeof(tempfile), "/tmp/%.*s%d%d%d.jpg", caps[0].len, caps[0].ptr, level, column, row); 
          stbi_write_jpg(tempfile, tilewidth, tileheight, samples_per_pixel, data, 85);
          delete [] data;
          struct mg_http_serve_opts opts = { .mime_types = "jpg=image/jpg" };
          mg_http_serve_file(c, hm, tempfile, &opts); 
      }
    }
 }

int main(int argc, char *argv[]) {
  char path[MG_PATH_MAX] = ".";
  struct mg_mgr mgr;
  struct mg_connection *c;
  int i;


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
  MG_INFO(("Khufu tile server version : v%s", "0.1"));
  MG_INFO(("Mongoose version : v%s", MG_VERSION));
  MG_INFO(("Listening on     : %s", s_listening_address));
  while (s_signo == 0) mg_mgr_poll(&mgr, 1000);
  mg_mgr_free(&mgr);
  MG_INFO(("Exiting on signal %d", s_signo));
  return 0;
}
