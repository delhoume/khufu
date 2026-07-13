VERSION=`cat Version`
OPTFLAGS =-O3
CXXFLAGS += -std=c++11 $(OPTFLAGS) -D KHUFU_VERSION=\"$(VERSION)\" -Wall -Wformat  -I src
SRCDIR = src

LIBS_DYNAMIC = -ltiff -ljpeg -lz -lzstd 

LIBS += $(LIBS_DYNAMIC)


all:  khufu

clean:
	rm -rf $(SRCDIR)/*~ $(SRCDIR)/*# 
	rm -rf *.o

khufu: khufu.o  mongoose.o
	$(CXX)   $(CXXFLAGS) khufu.o mongoose.o -o khufu $(LIBS)
	
khufu.o: $(SRCDIR)/khufu.cpp $(SRCDIR)/show_template.h
	$(CXX) $(CXXFLAGS) $(SRCDIR)/khufu.cpp -c -o khufu.o 

mongoose.o: $(SRCDIR)/mongoose.c
	$(CXX) $(CXXFLAGS) $(SRCDIR)/mongoose.c -c -o mongoose.o

# docker_image:
#   podman image rm -f  docker.io/delhoume/khufu
# 	podman container stop Khufu 
# 	podman container rm Khufu 
# 	podman build --no-cache -t docker.io/delhoume/khufu -f Dockerfile .
# 	podman push docker.io/delhoume/khufu
#   podman run -p 3333:8000 -d --name Khufu --env EXTERNALPORT=3333 -v C:\Users\FredericDelhoume\gitperso\osd_screen_wall\tiff:/mnt/tifroot docker.io/delhoume/khufu
