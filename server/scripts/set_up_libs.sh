#!/bin/bash

LIBS=$PWD/lib

for i in $( find $PWD/extras/ -maxdepth 2 -name lib -type d ); do
	echo $i
	LIBS=$LIBS:$i
done

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LIBS
export WHEATIS_LIBS_SET=true
