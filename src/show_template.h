#ifndef SHOW_TEMPLATE_H
#define SHOW_TEMPLATE_H

const char *show_template = "<html>\n\
<meta name=\"referer\" content=\"origin\">\n\
<head>\n\
  <title>Khufu viewer</title>\n\
<style>\n\
#openseadragon1 {\n\
  width: 100vw;\n\
  height: 100vh;\n\
}\n\
body {\n\
  margin: 0;\n\
  padding: 0;\n\
}\n\
</style>\n\
</head>\n\
<body>\n\
<div id=\"openseadragon1\"></div>\n\
</body>\n\
<script src=\"/app/openseadragon.min.js\"></script>\n\
<script type=\"text/javascript\">\n\
const SERVER = \"http://localhost:%d\";\n\
    const DIRFULL = %d;\n\
    const IMAGE = \"%s\";\n\
    const  WIDTH = %d;\n\
    const  HEIGHT = %d;\n\
    const  TILESIZE = %d;\n\
    const  MINLEVEL = %d;\n\
    const  MAXLEVEL = %d;\n\
    var viewer = OpenSeadragon({\n\
      id: \"openseadragon1\",\n\
      prefixUrl: \"/app/images/\",\n\
      showNavigator: true,\n\
      tileSources: { \n\
        width: WIDTH,\n\
        height: HEIGHT,\n\
        tileSize: TILESIZE, \n\
        minLevel: MINLEVEL,\n\
        maxLevel: MAXLEVEL,\n\
        overlay: 0,\n\
        getTileUrl: function( level, x, y ) {\n\
          // dirfull is either 0 or last index of directories (full is last)\n\
          const lvl = DIRFULL == 0 ? (MAXLEVEL - level) : DIRFULL - (MAXLEVEL- level);\n\
          return `${SERVER}/tile/${IMAGE}/${lvl}/${x}/${y}`;\n\
        }\n\
    }\n\
  });\n\
  </script>\n\
</html>\n";

#endif