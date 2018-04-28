#!/bin/bash
if [ -n "$1" ] ; then
   export CROSS_COMPILE=$1
fi

if [ "$CROSS_COMPILE" == "ppc_85xx-" ] ; then
   tar -xzf tinyxml-ppc.tar.gz
else
   tar -xzf tinyxml-intel.tar.gz
fi
