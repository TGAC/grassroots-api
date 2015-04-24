#!/bin/bash

if [ "$WHEATIS_LIBS_SET" != 'true' ]; then
	. ./set_up_libs.sh
fi

apachectl $1
