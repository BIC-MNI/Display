#! /bin/sh

if [ ! -r m4/mni_REQUIRE_LIB.m4 ]; then
    cat <<EOF
The required m4 files were not found.
You need to check these out from their repository
using

    cvs -d /software/source checkout -d m4 libraries/mni-acmacros

(yes, two '-d' options)
Then re-run autogen.sh.
EOF
    exit 1
fi

set -e
aclocal -I m4
autoheader
automake --add-missing
autoconf

