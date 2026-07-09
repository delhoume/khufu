VERSION=0.5
OPTFLAGS =-g
CXXFLAGS += -std=c++26 $(OPTFLAGS) -D KHUFU_VERSION=\"$(VERSION)\" -Wall -Wformat  -I src
#CXX=/opt/homebrew/bin/g++-13
PLATFORM = linux_x64

SRCDIR = src

LIBS_DYNAMIC = -ltiff -ljpeg -lz -lzstd 

LIBS += $(LIBS_DYNAMIC)


all:  khufu

clean:
	rm -rf $(SRCDIR)/*~ $(SRCDIR)/*# 
	rm -rf *.o

khufu: khufu.o  mongoose.o
	$(CXX)   $(CXXFLAGS) khufu.o mongoose.o -o khufu $(LIBS)
	
khufu.o: $(SRCDIR)/khufu.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/khufu.cpp -c -o khufu.o 

mongoose.o: $(SRCDIR)/mongoose.c
	$(CXX) $(CXXFLAGS) $(SRCDIR)/mongoose.c -c -o mongoose.o