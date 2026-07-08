FROM alpine:latest as builde


RUN apk update \
  && apk upgrade \
  && apk add --no-cache \
	  git \
	  make \
    g++ \
	  tiff-dev

WORKDIR /build
RUN git clone https://github.com/delhoume/khufu.git
RUN echo `pwd`
RUN echo `ls -la`

RUN cd khufu && make -f Makefile.alpine && strip ./khufu
RUN echo `pwd`
RUN echo `ls -la`
COPY openseadragon-bin-6.0.2/openseadragon.min.js openseadragon-bin-6.0.2/images /build/openseadragon/
COPY --chmod=0755 bin/khufu /build/

RUN echo `pwd`
RUN echo `ls -la /build`

# second stage
FROM alpine:latest


# Install runtime dependencies
# libtiff not available as static library, so we need to install it in the final image

RUN apk update \
  && apk upgrade \
  && apk add --no-cache \ 
  libstdc++ tiff

RUN mkdir -p /app/openseadragon
COPY --from=builder /build/openseadragon /app/openseadragon/
COPY --from=builder /build/khufu /app/
WORKDIR /app
ENTRYPOINT ["/app/khufu" "-d" "/mnt/webroot" "-f" "/mnt/tifroot" "-p" "8000" ]          