#!/bin/bash
if [ -n "$1" ] ; then
   export CROSS_COMPILE=$1
fi
rm -rf ./tinyxml
rm -rf ./include
rm -rf ./lib
rm -rf ./bin
tar xvfz tinyxml_2_5_2.tar.gz
cd tinyxml

sed -i -r -e "s/all: (.*)$/all: \1 lib/" Makefile

cat >>Makefile  <<EOF

LIB     := libtinyxml.a
LIBOBJS := tinyxml.o tinyxmlerror.o tinyxmlparser.o tinystr.o
INCLUDE := tinyxml.h tinystr.h

lib: \${LIB}

\${LIB}: \${LIBOBJS}
	\${AR} \${LIB} \${LIBOBJS}
EOF

if [ "$CROSS_COMPILE" == "ppc_85xx-" ] ; then
   export CC=ppc-linux-gcc
   export CXX=ppc-linux-g++
   export LD=ppc-linux-g++
   export RANLIB=ppc-linux-ranlib
   export CFLAGS=-fsigned-char
   make -e; ../../build.sh status $?
else
   make;  ../../build.sh status $?
fi

TINYXML_USE_STL=`grep '^TINYXML_USE_STL := YES' Makefile | cut -d' ' -f1`

cd ..
mkdir lib
cp ./tinyxml/libtinyxml.a ./lib
mkdir include
cp ./tinyxml/tinystr.h ./include

if [ "x$TINYXML_USE_STL" == "x" ] ; then
 	cp ./tinyxml/tinyxml.h ./include
else
	echo "#define TIXML_USE_STL 1" >include/tinyxml.h
	cat tinyxml/tinyxml.h  >>include/tinyxml.h
fi
