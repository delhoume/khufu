VERSION=0.5
OPTFLAGS = -O3
CXXFLAGS += -std=c++26 -D KHUFU_VERSION=\"$(VERSION)\" -Wall -Wformat  -I src
#CXX=/opt/homebrew/bin/g++-13
PLATFORM = linux_x64

SRCDIR = src
BINDIR = binlocalhost

LIBS_DYNAMIC =   -ltiff -lturbojpeg -ljpeg -lz -lzstd 

LIBS += $(LIBS_DYNAMIC)
PROGRAM = $(BINDIR)/khufu_$(VERSION)_$(PLATFORM)
#CXX=clang

all: $(BINDIR) $(PROGRAM)

clean:
	rm -rf $(SRCDIR)/*~ $(SRCDIR)/*# 
	rm -rf $(PROGRAM) $(BINDIR)/*.o

$(BINDIR):
	mkdir -p bin

$(PROGRAM): $(BINDIR)/khufu.o  $(BINDIR)/mongoose.o
	$(CXX) -v  $(CXXFLAGS) $(BINDIR)/khufu.o $(BINDIR)/mongoose.o -o $(PROGRAM) $(LIBS)

$(BINDIR)/khufu.o: $(SRCDIR)/khufu.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/khufu.cpp -c -o $(BINDIR)/khufu.o 

$(BINDIR)/mongoose.o: $(SRCDIR)/mongoose.c
	$(CXX) $(CXXFLAGS) $(SRCDIR)/mongoose.c -c -o $(BINDIR)/mongoose.o 
