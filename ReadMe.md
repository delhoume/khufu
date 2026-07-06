## khufu, a minimal but effecive tile and http server

### features
 - serves http
 - serves tiles rpom BigTiff with a standard Rest Api
 - very efficient (uses optimized dependencies)  
 - instat display of massive files
 - single file c++ source code, based on well known opensource- mongoose http library
 - no config


### Usage
```
bin/khufu_0.5_macos_arm64 --help
Khufu tile server version : v0.5
Usage: bin/khufu_0.5_macos_arm64 OPTIONS
  -d FOLDER - base folder, default: .
  -f FOLDER - folder with TIFF images, default: .
  -h [ADDR]   - listening address, default: '0.0.0.0'
  -p [PORT]   - listening port, default: '8000'
  -v LEVEL  - debug level, from 0 to 4, default: 2
```

- files in base folder are available through http 
- tiled (optiionaly pyramidal) TIFF are available though rest APIs:
   - `/info`returns a json description of available TIFF images by id 

   - `/show/{id}`  returns an html that leverages openseadragon JS library to display a TIFF by id

   - `/tile/{id}/{level}/{x}/{y}` returns an image of the tile of given pameters.