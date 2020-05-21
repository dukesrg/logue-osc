#!/bin/sh
([ "$#" -ne 2 ] || [ ! -f $1 ] || [ ! -f $2 ]) && echo \
"Usage: ${0##*/} <logue oscillator file> <mono 16-bit PCM or u-law WAV file or raw u-law data file>" && exit
NAME=${2##*/}
NAME=${NAME%.*}
mkdir -p $NAME
tar xvf $1 --strip=1 -C $NAME
INPUT="if=$2"
HEADER=$(od -An -tx4 -N36 -v $2 | tr -d '\n' | tr -s ' ' | cut -d' ' -f2,4,7,10)
if [ "$HEADER" = "46464952 45564157 00010001 00100002" ]; then
	echo Compatible PCM wave file detected, transcoding to u-Law...
	SKIP=44
	INPUT=""
elif [ "$HEADER" = "46464952 45564157 00010007 00080001" ]; then
	echo Compatible u-Law wave file detected
	SKIP=58
else
	echo Compatible PCM or u-law wave file NOT detected, treating as raw u-law data
	SKIP=0
fi
([ $SKIP -ne 44 ] || ./PCM2uLaw.sh $2) | dd conv=notrunc bs=1 $INPUT skip=$SKIP of=$NAME/payload.bin seek=64 count=16384
sed -E s'/^( *"name" *: *")[^"]+(.*)$/\1'${NAME:0:12}'\2/' $NAME/manifest.json > $NAME/manifest.tmp
mv -f $NAME/manifest.tmp $NAME/manifest.json
zip -mr $NAME.${1##*.} $NAME
