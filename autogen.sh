#! /bin/sh

set -e

libtoolize --automake --copy
aclocal
autoheader
automake --add-missing --copy
autoconf

