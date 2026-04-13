# cl /MD /out:khufu.exe src/mongoose.c src/khufu.cpp /Isrc /Isrc/tiff-4.3.0/libtiff /Isrc/zlib-1.2.11 /Isrc/libjpeg-turbo2.1.2/include  src/libs64/zlib.lib src/libs64/libtiff.lib src/libs64/turbojpeg-static.lib src/libs64/libwebp.lib
# cmake -G Ninja -D JPEG_INCLUDE_DIR=..\libjpeg-turbo64_3.1.0\include -D JPEG_LIBRARY=..\libjpeg-turbo64_3.1.0\lib\turbojpeg-static.lib -D zlib=ON -D ZLIB_LIBRARY=..\zlib-1.3.2\zlib.lib -D BUILD_SHARED_LIBS=OFF -D ZLIB_INCLUDE_DIR=..\zlib-1.3.2 -D logluv=OFF -D CMAKE_BUILD_TYPE=Release
#cmake --bui
CC=cl
LD=link
SRCDIR= src

COMMONLIBSDIR = deps
TIFF = $(COMMONLIBSDIR)/tiff-4.7.1/libtiff
TIFFFLAGS = /I$(TIFF)
#TIFFLIB = $(TIFF)\tiffd.lib
TIFFLIB = $(TIFF)\tiff.lib

ZLIB = $(COMMONLIBSDIR)/zlib-1.3.2
ZLIBFLAGS = /I$(ZLIB)
ZLIBLIB = $(ZLIB)\zlib.lib


JPEG = $(COMMONLIBSDIR)/libjpeg-turbo64_3.1.0
JPEGFLAGS = /I$(JPEG)\include
JPEGLIB = $(JPEG)\lib\turbojpeg-static.lib


# DEBUG=/Od /Zi
DEBUG=/O2

# LDDEBUG=/DEBUG"
VERSION=0.4
COMMMONLIBSFLAGS = $(TIFFFLAGS) $(JPEGFLAGS) $(ZLIBFLAGS)
COMMONLIBS = $(TIFFLIB) $(JPEGLIB) $(ZLIBLIB) 
CFLAGS = /std:c++20 /Inologo /MD /W3 $(COMMMONLIBSFLAGS) $(DEBUG) /DKHUFU_VERSION=\"$(VERSION)\" /D_CRT_SECURE_NO_DEPRECATE  /I. /Isrc
LDFLAGS = $(LDDEBUG) /SUBSYSTEM:CONSOLE /nodefaultlib:libc 

SYSLIBS = wininet.lib user32.lib gdi32.lib kernel32.lib comctl32.lib comdlg32.lib shlwapi.lib \
	  shell32.lib advapi32.lib version.lib strsafe.lib msimg32.lib winmm.lib


OBJECTS = khufu.obj mongoose.obj
PLATFORM = win_amd64

all: khufu$(VERSION)_$(PLATFORM).exe

khufu$(VERSION)_$(PLATFORM).exe: $(OBJECTS)
	$(LD) /OUT:bin/khufu_$(VERSION)_$(PLATFORM).exe $(LDFLAGS) $(OBJECTS) $(SYSLIBS) $(COMMONLIBS)
	
khufu.obj: $(SRCDIR)/khufu.cpp
	$(CC) $(CFLAGS) /c $(SRCDIR)/khufu.cpp

mongoose.obj: $(SRCDIR)/mongoose.c $(SRCDIR)/mongoose.h
	$(CC) $(CFLAGS) /c $(SRCDIR)/mongoose.c

clean:
	del *.obj
