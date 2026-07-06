// Copyright (c) 2024 Frederic Delhoume
// All rights reserved


#include <iostream>
#include <cstring>
#include <cmath>

using namespace std;

extern "C" {
#include <tiffio.h>
}

static void Usage() {
           std::cout  << "Usage: pyramid2khufu <name>.tif" << std::endl <<
                     "     sed replacement file generator" << std::endl <<            
                    "     F. Delhoume 2024 delhoume@gmail.com" << std::endl;
 
}

// simple generator for sed replacement file to use with khufu.tpl.html
int 
main(int argc, char* argv[]) {
    if (argc == 1) {
        Usage();
        return 1;
    }
    TIFFSetErrorHandler(NULL);
    TIFFSetErrorHandlerExt(NULL);
   TIFFSetWarningHandler(NULL);
    TIFFSetWarningHandlerExt(NULL);

   const char* name = argv[1];

    char filename[128];
    sprintf(filename, "%s.tif", argv[1]); 
    TIFF* tifin = TIFFOpen(filename, "r");
    if (!tifin) {
        Usage();
        return 1;
    }
    int directories = TIFFNumberOfDirectories(tifin);
    // find the one that is not reduced
    for (unsigned int idx = 0; idx < directories; ++idx) {
        TIFFSetDirectory(tifin, idx);
        unsigned int filetype;
        TIFFGetFieldDefaulted(tifin, TIFFTAG_SUBFILETYPE, &filetype);
        if (!(filetype & FILETYPE_REDUCEDIMAGE)) {
            // first found is used
            unsigned int maxlevel;
            unsigned int imagewidth;   
            unsigned int imageheight;
            unsigned int tilewidth;   
            unsigned int tileheight;
            TIFFGetField(tifin, TIFFTAG_IMAGEWIDTH, &imagewidth);
            TIFFGetField(tifin, TIFFTAG_IMAGELENGTH, &imageheight);
            TIFFGetField(tifin, TIFFTAG_TILEWIDTH, &tilewidth);
            TIFFGetField(tifin, TIFFTAG_TILELENGTH, &tileheight);
            unsigned int maxdimension = imagewidth;
            if (imageheight > imagewidth) maxdimension = imageheight;
                maxlevel = ceil(log(maxdimension)/log(2));
            std::cout << "s/IMAGE/" << name << "/g" << std::endl
            << "s/WIDTH/" << imagewidth << "/g" << std::endl
            << "s/HEIGHT/" << imageheight << "/g" << std::endl
            << "s/TILESIZE/" << tilewidth << "/g" << std::endl
            << "s/MINLEVEL/" << (maxlevel - directories + 1) << "/g" << std::endl   
            << "s/MAXLEVEL/" << maxlevel << "/g" << std::endl
            << "s/DIRFULL/" << idx << "/g" << std::endl;
            TIFFClose(tifin);
            return 0;
        }
    }
    TIFFClose(tifin);
    return 0;
}
