#! /bin/sh

set -e

aclocal -I m4
autoheader
automake --add-missing
autoconf

