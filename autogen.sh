#! /bin/sh

aclocal \
&& autoconf \
&& automake -o ./build -a 
