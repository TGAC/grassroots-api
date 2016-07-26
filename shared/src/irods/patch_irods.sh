#!/bin/sh

cp patches/server.config.patch $IRODS_HOME/server/config
cd $IRODS_HOME/server/config
patch < server.config.patch
rm server.config.patch
