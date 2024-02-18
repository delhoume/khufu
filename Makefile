CCC=/opt/homebrew/bin/g++-13

CCFLAGS = -std=c++11  -O3 -I /opt/homebrew/include -I .


SRCDIR = src
BINDIR = bin

LIBS = /opt/homebrew/lib/libtiff.a  /opt/homebrew/lib/libturbojpeg.a \
    /opt/homebrew/lib/libz-ng.a /opt/homebrew/lib/libzlibstatic.a /opt/homebrew/lib/libzstd.a /opt/homebrew/lib/liblzma.a

PROGRAMS = $(BINDIR)/khufu $(BINDIR)/tiff2khufu

all: $(BINDIR) $(PROGRAMS)

clean:
	rm -rf *~ *# .??*
	rm -rf $(SRCDIR)*~ $(SRCDIR)*# $(SRCDIR).??*
	rm -rf $(PROGRAMS)

$(BINDIR):
	mkdir -p bin

$(BINDIR)/khufu: $(SRCDIR)/khufu.cpp $(BINDIR)
	$(CCC) $(CCFLAGS) $(SRCDIR)/khufu.cpp $(SRCDIR)/mongoose.c -o $(BINDIR)/khufu $(LIBS)

$(BINDIR)/tiff2khufu: $(SRCDIR)/tiff2khufu.cpp $(BINDIR)
	$(CCC) $(CCFLAGS) $(SRCDIR)/tiff2khufu.cpp -o $(BINDIR)/tiff2khufu $(LIBS)
