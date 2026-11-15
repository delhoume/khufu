## khufu, a minimal but effective tile and http server

### Features

- serves http from a specfic folder.
- serves tiles from BigTiff with standard Rest Api
- has builtin catalog and full image viewing capabilities based on opensource OpenSeadragon JS library
- very efficient (uses optimized dependencies)
- instant display of massive files
- single file c++ source code, based on opensource mongoose http library

- https://github.com/cesanta/mongoose

- https://github.com/openseadragon/openseadragon

### Usage

```
>khufu --help
Khufu tile and web server version : v1.0
Usage: ./khufu OPTIONS
  -d FOLDER - web folder, default: .
  -f FOLDER - folder with TIFF images, default: .
  -h [ADDR]   - listening address, default: '0.0.0.0'
  -p [PORT]   - listening port, default: '8000'
  -v LEVEL  - log level, one of NONE|ERROR|INFO(default)|DEBUG|VERBOSE
```

- files in web folder are available through http
- tiled (optionally pyramidal) TIFFs in TIFF folder are available though rest APIs:
  - `/info`returns a json description of available ids in TIFF folder

  - `/tile/{id}/{level}/{x}/{y}[.png,.jpg]` returns an image of the tile at given location in specified format (default is JPG)

  An OSD TileSource is defined in ` demo/show.html`

### Docker

Khufu is available as a standalone docker container.
Just tell it where to find TIFF files and you are ready to view gigantic images locally. TIFFs must be tiled to be viewable.

podman (or docker)usage on Windows :

```
podman run -p 3333:8000 -d --name Khufu --env EXTERNALPORT=3333 -v C:\path_to_tiled_tiffs:/mnt/tifroot docker.io/delhoume/khufu
```

then point your browser to `http://localhost:3333` and voila !
