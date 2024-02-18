#!/bin/bash

INPUT=$1

KHUFU_FOLDER=/Users/fredericdelhoume/Documents/GitHub/khufu

BASE_FOLDER=$(dirname $1)
BASE_INPUT=$(basename $1 .tif)

pushd $BASE_FOLDER
$KHUFU_FOLDER/bin/tiff2khufu $BASE_INPUT > /tmp/khufu_$BASE_INPUT.sed 
sed -f /tmp/khufu_$BASE_INPUT.sed $KHUFU_FOLDER/khufu.tpl.html > $BASE_INPUT.html
$KHUFU_FOLDER/bin/khufu
popd