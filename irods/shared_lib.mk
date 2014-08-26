export MY_CFLAG=-fPIC -g -pg

IRODS = $(shell echo "$$IRODS_HOME")

.PHONY:	shared


include Makefile

shared: clean all
	gcc --shared  -o libirods.so $(OBJS) 
	
