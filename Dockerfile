FROM alpine:latest


RUN apk update \
  && apk upgrade \
  && apk add --no-cache \
    clang \
    clang-dev \
	git \
	make \
	g++ \
	tiff-dev


	WORKDIR /code

git clone https://github.com/delhoume/khufu.git

cd khufu 
make
