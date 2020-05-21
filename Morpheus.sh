#!/bin/sh
unzip $1 Morpheus/payload.bin
dd conv=notrunc bs=1 if=$2 of=Morpheus/payload.bin seek=64 count=16384
zip -mr $1 Morpheus
