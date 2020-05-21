#!/bin/sh
([ "$#" -ne 1 ] || [ ! -f $1 ]) && echo "Usage: ${0##*/} <logue oscillator file>" && exit
mkdir -p WaveEdit
curl https://waveeditonline.com/wav-files.zip | tar -xvf- -C WaveEdit
for i in WaveEdit/*.WAV; do
	./Morpheus.sh $1 $i &
done
