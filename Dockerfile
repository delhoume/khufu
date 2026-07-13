FROM alpine:latest as builder


RUN apk update \
  && apk upgrade \
  && apk add --no-cache \
	  git \
	  make \
    g++ \
	  tiff-dev

WORKDIR /build
RUN git clone https://github.com/delhoume/khufu.git

RUN cd khufu && make

COPY khufu /build/
COPY openseadragon-bin-6.0.2/openseadragon.min.js /build
COPY app /build/

# second stage
FROM alpine:latest


# Install runtime dependencies
# libtiff not available as static library, so we need to install it in the final image

RUN apk update \
  && apk upgrade \
  && apk add --no-cache \ 
  libstdc++ tiff

#RUN apk add gdb

RUN mkdir -p /app/openseadragon
COPY --from=builder /build/openseadragon /app/openseadragon/
COPY --from=builder /build/app /app/
COPY --from=builder --chmod=0755 /build/khufu /app/

EXPOSE 8000
WORKDIR /app
ENTRYPOINT ["./khufu"]
CMD ["-d", "/mnt/webroot", "-f", "/mnt/tifroot", "-p", "8000" ]
# ENTRYPOINT ["sh"]  