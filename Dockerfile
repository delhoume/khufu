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
    cd khufu && make 

FROM alpine:latest

COPY --from=build-stage /build/khufu/bin/khufu_linux_x64 /usr/local/bin/khufu

USER 1000
WORKDIR /
HEALTHCHECK NONE
ENTRYPOINT ["/usr/local/bin/khufu"]