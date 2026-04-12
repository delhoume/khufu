
OPTFLAGS = -O3
CXXFLAGS += -std=c++11 $(OPTFLAGS)  -v -Wall -Wformat -I/usr/local/include -I/opt/local/include -I /opt/homebrew/include -I src
#CXX=/opt/homebrew/bin/g++-13


SRCDIR = src
BINDIR = bin

LIB_BREW = /opt/homebrew/lib
LIB_ZLIBNGCOMPAT=/opt/homebrew/opt/zlib-ng-compat/lib
LIBS_DYNAMIC = -L $(LIB_BREW) -L $(LIB_ZIBNGCOMPAT) -ltiff -lturbojpeg -lz-ng -lzstd -llzma

LIBS_STATIC = $(LIB_BREW)/libtiff.a $(LIB_BREW)/libturbojpeg.a $(LIB_ZLIBNGCOMPAT)/libz.a $(LIB_BREW)/libzstd.a $(LIB_BREW)/liblzma.a
# LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
LIBS += $(LIBS_STATIC)
# LIBS += $(LIBS_DYNAMIC)
PROGRAMS = $(BINDIR)/khufu $(BINDIR)/tiff2khufu

all: $(BINDIR) $(PROGRAMS)

clean:
	rm -rf $(SRCDIR)/*~ $(SRCDIR)/*# 
	rm -rf $(PROGRAMS) $(BINDIR)/*.o $(BINDIR)/ingui.ini


$(BINDIR):
	mkdir -p bin

$(BINDIR)/khufu: $(BINDIR)/khufu.o  $(BINDIR)/mongoose.o
	$(CXX) -v  $(CXXFLAGS) $(BINDIR)/khufu.o $(BINDIR)/mongoose.o -o $(BINDIR)/khufu $(LIBS)

$(BINDIR)/khufu.o: $(SRCDIR)/khufu.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/khufu.cpp -c -o $(BINDIR)/khufu.o 

$(BINDIR)/mongoose.o: $(SRCDIR)/mongoose.c
	$(CXX) $(CXXFLAGS) $(SRCDIR)/mongoose.c -c -o $(BINDIR)/mongoose.o 



$(BINDIR)/tiff2khufu: $(BINDIR)/tiff2khufu.o 
	$(CXX) -v  $(CXXFLAGS) $(BINDIR)/tiff2khufu.o -o $(BINDIR)/tiff2khufu $(LIBS)

$(BINDIR)/tiff2khufu.o: $(SRCDIR)/tiff2khufu.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/tiff2khufu.cpp -c -o $(BINDIR)/tiff2khufu.o 
