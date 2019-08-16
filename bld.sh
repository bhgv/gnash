#!/bin/sh


cd external/agg-2.5
make -j4
make clean
cd ../..


#fb
#./configure --without-gconf --prefix=$PWD/out --enable-docbook --enable-log --enable-cygnal \
#	--enable-media=ffmpeg --disable-npapi --enable-renderer=agg \
#	--with-agg-incl=$PWD/external/agg-2.5/include --with-agg-lib=$PWD/external/agg-2.5/lib \
#	--disable-rpath \
#	--enable-device=rawfb \
#	--enable-gui=fb \
#	--enable-sound=sdl --disable-jemalloc

#sdl
#./configure --prefix=$PWD/out --enable-docbook --enable-log --enable-cygnal --enable-media=ffmpeg --disable-npapi --enable-renderer=agg --with-agg-incl=$PWD/../agg/agg-2.5/include --with-agg-lib=$PWD/../agg/agg-2.5/lib --disable-rpath --enable-gui=sdl --enable-sound=sdl --disable-jemalloc

#gtk
./configure --prefix=$PWD/out --enable-docbook --enable-log --enable-cygnal --enable-media=ffmpeg --disable-npapi --enable-renderer=agg --with-agg-incl=$PWD/external/agg-2.5/include --with-agg-lib=$PWD/external/agg-2.5/lib --disable-rpath --enable-gui=gtk --enable-sound=sdl --disable-jemalloc


make -j4
