#!/bin/sh

if [ ! -e Pharo.image ]; then
    curl https://get.pharo.org/70+vm | bash
fi

rm -rf pharo-local
rm -rf mc
mkdir -p mc
./pharo Pharo.image st scripts/tonelToMonticello.st
