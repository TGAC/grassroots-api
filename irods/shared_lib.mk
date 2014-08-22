export MY_CFLAG=-fPIC

include Makefile

.PHONY:	shared

shared: clean all
	gcc --shared  -o libirods.so $(OBJS) 
