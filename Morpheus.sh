#!/bin/sh
([ "$#" -ne 2 ] || [ ! -f $1 ] || [ ! -f $2 ]) && echo "Usage: ${0##*/} <logue oscillator file> <u-law compressed WAV or RAW file>" && exit
SKIP=0
# RIFF WAVE mono u-law
[ "46464952 45564157 00010007" = "$(od -An -tx4 -N24 -v $2 | tr -d '\n' | tr -s ' ' | cut -d' ' -f2,4,7)" ] && SKIP=58
unzip $1
NAME=${2##*/}
NAME=${NAME%.*}
NAME=${NAME:0:12}
sed -E s'/^( *"name" *: *")[^"]+(.*)$/\1'$NAME'\2/' Morpheus/manifest.json > Morpheus/manifest.new
mv -f Morpheus/manifest.new Morpheus/manifest.json
dd conv=notrunc bs=1 if=$2 skip=$SKIP of=Morpheus/payload.bin seek=64 count=16384
zip -mr "${2%.*}.${1##*.}" Morpheus
