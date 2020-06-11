#!/bin/sh
([ "$#" -lt 2 ] || [ "$#" -gt 5 ]) && echo \
"Usage: ${0##*/} <logue oscillator file> <minilogue/monologue prog compilation>" && exit
SRC=$1
NAME=${2##*/}
NAME=${NAME%.*}
mkdir -p $NAME
tar xvf $SRC --strip=1 -C $NAME
SEEK=64
shift
while (( "$#" )); do
	echo Injecting $1...
	[ ! -f $1 ] && echo Error: file does not exist && exit
#	HEADER=$(od -An -tx1 -N6 -v $1 | tr -d ' ')
#	if [[ "$HEADER" = "f04300091000" || "$HEADER" = "f04300041000" ]]; then
#		echo Warning: malformed SysEx size ignored
#	elif [[ "$HEADER" != "f04300092000" && "$HEADER" != "f04300042000" ]]; then
#		echo Error: invalid Sysex header && exit
#        fi
	dd conv=notrunc bs=1 if=$1 skip=0 of=$NAME/payload.bin seek=$SEEK count=28672
	SEEK=$((SEEK + 448))
	shift
done
sed -E s'/^( *"name" *: *")[^"]+(.*)$/\1'${NAME:0:12}'\2/' $NAME/manifest.json > $NAME/manifest.tmp
mv -f $NAME/manifest.tmp $NAME/manifest.json
zip -mr $NAME.${SRC##*.} $NAME
