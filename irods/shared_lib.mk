export MY_CFLAG=-fPIC -g -pg

IRODS = $(shell echo "$$IRODS_HOME")

.PHONY:	shared

buildDir = $(IRODS)
include lib/Makefile

shared: lib
	gcc --shared  -o $(DIR_SHARED_IRODS_LIB)/libirods.so $(OBJS) 
	
