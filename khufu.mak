CC  = cl
LD  = link

/*
cl /MD /out:khufu.exe src/mongoose.c src/khufu.cpp /Isrc /Isrc/tiff-4.3.0/libtiff /Isrc/zlib-1.2.11 /Isrc/libjpeg-turbo2.1.2/include  src/libs64/zlib.lib src/libs64/libtiff.lib src/libs64/turbojpeg-static.lib src/libs64/libwebp.lib
*/

SRCDIR = src

COMMONLIBSDIR = C:\Users\076788706\Documents\GitHub\vlivplugins\internal

LIBS64 = $(COMMONLIBSDIR)/libs64

TIFF = $(COMMONLIBSDIR)/tiff-4.3.0\libtiff	
TIFFFLAGS = /I$(TIFF)
#TIFFLIB = $(TIFF)\libtiff.lib
TIFFLIB = $(LIBS64)/libtiff.lib

ZLIB = $(COMMONLIBSDIR)/zlib-1.2.11
ZLIBFLAGS = /I$(ZLIB)
#ZLIBLIB = $(ZLIB)\zlib.lib
ZLIBLIB = $(LIBS64)\zlib.lib

JPEG = $(COMMONLIBSDIR)/libjpeg-turbo2.1.2
JPEGFLAGS = /I$(JPEG)\include
#JPEGLIB = $(JPEG)\lib\turbojpeg-static.lib
JPEGLIB = $(LIBS64)\turbojpeg-static.lib

PNG = $(COMMONLIBSDIR)/lpng1637
PNGFLAGS = /I$(PNG)
#PNGLIB = $(PNG)\libpng.lib
PNGLIB = $(LIBS64)\libpng.lib

WEBP = $(COMMONLIBSDIR)/libwebp-0.4.1-windows-x86
WEBPFLAGS = /I$(WEBP)\include
#WEBPLIB = $(WEBP)\lib\libwebp.lib
WEBPLIB = $(LIBS64)\libwebp.lib

DEBUG=/Ox 
LDDEBUG=

#DEBUG=/Od /Zi
#LDDEBUG=/debug

CFLAGS = /nologo /W3 $(DEBUG) /nodefaultlib:libc /MD /D_NO_CRT_STDIO_INLINE /D_CRT_SECURE_NO_DEPRECATE /DWIN32 /DWINDOWS /I. 
LDFLAGS = $(LDDEBUG)

COMMMONLIBSFLAGS = $(TIFFFLAGS) $(PNGFLAGS) $(JPEGFLAGS) $(ZLIBFLAGS)
COMMONLIBS = $(TIFFLIB) $(JPEGLIB) $(ZLIBLIB) $(WEBLIB)

CFLAGS = /nologo /W3 $(COMMMONLIBSFLAGS) $(DEBUG) /D_CRT_SECURE_NO_DEPRECATE /DWIN32 /DWINDOWS /I. /Isrc
LDFLAGS = $(LDDEBUG) /SUBSYSTEM:WINDOWS /nodefaultlib:libc 

SYSLIBS = wininet.lib user32.lib gdi32.lib kernel32.lib comctl32.lib comdlg32.lib shlwapi.lib \
	  shell32.lib advapi32.lib version.lib strsafe.lib msimg32.lib winmm.lib


OBJECTS = khufu.obj mongoose.obj

all: khufu.exe

khufu.exe: $(OBJECTS)
	$(LD) /OUT:bin/khufu.exe $(LDFLAGS) $(OBJECTS) $(SYSLIBS) $(COMMONLIBS)

khufu.obj : $(SRCDIR)/khufu.cpp
	$(CC) $(CFLAGS) /c $(SRCDIR)/khufu.cpp

mongoose.obj : $(SRCDIR)/mongoose.c $(SRCDIR)/mongoose.h
	$(CC) $(CFLAGS) /c $(SRCDIR)/mongoose.c
