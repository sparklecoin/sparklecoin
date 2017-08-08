#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/sparkleunity.png
ICON_DST=../../src/qt/res/icons/sparkleunity.ico
convert ${ICON_SRC} -resize 16x16 sparkleunity-16.png
convert ${ICON_SRC} -resize 32x32 sparkleunity-32.png
convert ${ICON_SRC} -resize 48x48 sparkleunity-48.png
convert sparkleunity-48.png sparkleunity-32.png sparkleunity-16.png ${ICON_DST}

