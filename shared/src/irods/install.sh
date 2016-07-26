#!/bin/sh

cp -r -u -f modules/* $IRODS_HOME/modules/
cp -r -u -f server/* $IRODS_HOME/server/
cp -u -f util/debug/tgac_irods_util/*.so /opt/iRods-3.3.1/lib/
