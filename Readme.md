  # **Khufu** is a very simple HTTP server that serves tiles from  **single pyramidal TIFF files**

This allows gigantic TIFF files to be deployed over the Web, without the hassle 
of generating a Deep Zoom file hierarchy and having a web server deliver it.

When you have terapixel images, with dimensions up to 4 267 800 x 2 086 480 pixels (as from htts://github.com/delhoume/BigMars), even with a 512x512 tile size you end up having 5558 x 3705 jpeg files (that makes more than 2 million files for the most detailed level).

You would deploy then using a DZI file that describes the image and can be visualized in a web browser,
with a web server serving the tiles, and an OpenSeaDragon javascript object in an HTML page.

**Khufu** allows tiles to be be served from a single TIFF that has the same structure as a DeepZoom image.

Thanks to the wonderful OpenSeaDragon project, all you have to do is use a Custom Embedded Source in an HTML
page, run the **khufu** Web Server were your images are, and your image is instantly viewable, whatever its dimensions.

```bin/khufu```

You then can open locally the corresponding HTML file for your image.
You may have to update the SERVER variable  in the HTML depending on your configuration.
You will also need the openseadragon.min.js file.

````
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0">
<style>
#openseadragon1 { width: 100vw; height: 100vh; }
body { margin: 0; padding: 0; }
</style>

<div id="openseadragon"></div>
<script src="openseadragon.min.js"></script>
<script type="text/javascript">
const SERVER = "http://0.0.0.0:8000";
var viewer = OpenSeadragon({
  id: "openseadragon",
  prefixUrl: "https://cdn.jsdelivr.net/npm/openseadragon@3.1.0/build/openseadragon/images/",
  tileSources: {
        overlay: 0,
        width:  189680,
        height: 189680,
        tileSize: 512,
        minLevel: 9,
        maxLevel: 18
        getTileUrl: function( level, x, y ) {
            return SERVER + "/tile/"  + mars_4_4 + "/" + (18 - level) + "/" + x + "/" + y;
        },
    //  debugMode: true
}});
</script>
````

and you can display your gigantic image.

You can generate this HTML from a template (for ```mars_4_4.tif```)
```bin/tiff2khufu mars_4_4 | sed -f - khufu.tpl.html > mars_4_4.html```

Khufu is built with the great Mongoose library https://mongoose.ws/ (two files and you have a web server !),
Libtiff (https://libtiff.gitlab.io/libtiff/#) and STB (https://github.com/nothings/stb/blob/master/stb_image_write.h).
And visualization provided by the fantastic OpenSeaDragon project (https://openseadragon.github.io/)

The source code for both the server (```src/khufu.cpp``` and the sed parameter generator
```src/tiff2khufu.cpp``` is very simple.

I develop on a Mac Mini 2020  with Homebrew to fetch dependencies. Go to https://brew.sh, follow instructions to install, go to the khufu directory then type:
```
brew install gcc libtiff jpeg-turbo zlib-ng zstd lzma
make
```

and you are ready to go

![khufu sserving tiles of a 200 000 x 200 000 single TIFF for a local HTML file](images/cassini.png)


  ## Issues

 for each tile request I generate a file on disk and use mg_http_serve_file, this is ineficient and there might
 be issues concurrently acccessing the same file with multiple instances of khulu running or even multithreading.

If you know how to serve images from memory with Mongoose this would be very apprecited.

The protocol is very simple
Requests are in the URI as: ```/tile/<name>/<level>/<col>/<row>```

```http://127.0.0.1:8000/tile/cassini/0/0/0``` will retrieve the top-left tile of the first directory of the cassini.tif image

khufu retrieves tile at col and row for the given level (starting from 0) TIFF image directory and returns a Jpeg file, so all level or row/column mappings must be present in the OpenSeaDragon config in the HTML.


(```tiff2khufu``` generates this for you)
```bin/tiff2khufu mars_4_4 |  sed -f - khufu.tpl.html > mars_4_4.html```

For alternative to khufu, you can on Windows use my **Vliv** viewer for parmidal TIFFS.
https://github.com/delhoume/vliv
