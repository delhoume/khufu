FROM alpine:latest as build-stage

WORKDIR /build

RUN apk update \
  && apk upgrade \
  && apk add --no-cache \
    clang \
    clang-dev \
	  git \
	  make \
    g++ \
	 tiff-dev


RUN git clone https://github.com/delhoume/khufu.git  && \
    cd khufu && make && strip bin/khufu
COPY khufu/openseadragon-bin-6.0.2/openseadragon.min.js /khufu/openseadragon-bin-6.0.2/images /app/openseadragon/
 
WORKDIR /app

# Install runtime dependencies
# libtiff not available as static library, so we need to install it in the final image

RUN apk update \
  && apk upgrade \
  && apk add --no-cache \ 
  libgcc libstdc++ tiff

COPY --from=build-stage /build/khufu/openseadragon /app/openseadragon/
COPY --from=build-stage /build/khufu/bin/khufu /app/khufu  
USER 1000
ENTRYPOINT ["/app/khufu" "-d" "/mnt/webroot" "-f" "/mnt/tifroot" "-p" "8000" ]          