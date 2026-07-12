## khufu, a minimal but effective tile and http server

### Features
 - serves http from a specfic folder.
 - serves tiles from BigTiff with  standard Rest Ap
 - has builtin catalog and full image viewing capabilities based on opensource OpenSeadragon JS library
 - very efficient (uses optimized dependencies)  
 - instant display of massive files
 - single file c++ source code, based on opensource mongoose http library
 - almost no config

- https://github.com/cesanta/mongoose

- https://github.com/openseadragon/openseadragon

### Usage
```
>khufu --help
Khufu tile server version : v1.0
Usage: ./khufu OPTIONS
  -d FOLDER - web folder, default: .
  -f FOLDER - folder with TIFF images, default: .
  -h [ADDR]   - listening address, default: '0.0.0.0'
  -p [PORT]   - listening port, default: '8000'
  -v LEVEL  - log level, one of NONE|ERROR|INFO(default)|DEBUG|VERBOSE
```

- files in web folder are available through http 
- tiled (optionaly pyramidal) TIFF in TIFF foder are available though rest APIs:

   - `/info`returns a json description of available ids in TIFF folder

   - `/show/{id}`  returns an html that leverages openseadragon JS library to display a TIFF by id

   - `/tile/{id}/{level}/{x}/{y}` returns a JPEG  image of the tile at given location. 

   The OSD TileSource is defined in ` src/show_template.h` 


### Docker
Khufu is available as a standalone docker container.
Just tell it where to find TIFF files and you are ready to view gigantic images locally. TIFFs must be tiled to be viewable.

podman usage on Windows :
````
podman run -p 3333:8000 -d --name Khufu --env EXTERNALPORT=3333 -v C:\path_to_tiled_tiffs:/mnt/tifroot docker.io/delhoume/khufu
````

then point your browser to `http://localhost:3333/app/index.html` and voila !
