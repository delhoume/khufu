FROM alpine:latest as build-stage

WORKDIR /build

RUN apk update \
  && apk upgrade \
  && apk add --no-cache \
	  git \
	  make \
    g++ \
	  tiff-dev


RUN git clone https://github.com/delhoume/khufu.git  && \
    cd khufu && make && strip bin/khufu

COPY openseadragon-bin-6.0.2/openseadragon.min.js openseadragon-bin-6.0.2/images /build/openseadragon/
COPY /build/khufu/bin/khufu /build/khufu

# second stage
FROM alpine:latest


# Install runtime dependencies
# libtiff not available as static library, so we need to install it in the final image

RUN apk update \
  && apk upgrade \
  && apk add --no-cache \ 
  libstdc++ tiff

COPY --from=build-stage /build/openseadragon /app/openseadragon/
COPY --from=build-stage /build/khufu /app/khufu  
USER 1000
WORKDIR /app
HEALTHCHECK NONE
ENTRYPOINT ["/app/khufu" "-d" "/mnt/webroot" "-f" "/mnt/tifroot" "-p" "8000" ]          