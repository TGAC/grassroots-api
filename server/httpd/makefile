SHELL = /bin/bash

NAME 		= wheatis
DIR_SRC = src
DIR_INCLUDE = include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF

BUILD		= debug

DIR_OBJS = $(BUILD)

DIR_IRODS	= $$IRODS_HOME
DIR_APACHE_MODULES = /usr/local/apache2/modules


DIR_WHEATIS_UTIL = ../../util
DIR_WHEATIS_UTIL_INC = $(DIR_WHEATIS_UTIL)/include
DIR_WHEATIS_UTIL_LIB = $(DIR_WHEATIS_UTIL)/$(BUILD)
WHEATIS_UTIL_LIB_NAME = wheatis_util

DIR_WHEATIS_NETWORK = ../../network
DIR_WHEATIS_NETWORK_INC = $(DIR_WHEATIS_NETWORK)/include
DIR_WHEATIS_NETWORK_LIB = $(DIR_WHEATIS_NETWORK)/$(BUILD)
WHEATIS_NETWORK_LIB_NAME = wheatis_network

DIR_WHEATIS_SERVICES = ../../services/lib
DIR_WHEATIS_SERVICES_INC = $(DIR_WHEATIS_SERVICES)/include
DIR_WHEATIS_SERVICES_LIB = $(DIR_WHEATIS_SERVICES)/$(BUILD)
WHEATIS_SERVICES_LIB_NAME = wheatis_service

DIR_WHEATIS_IRODS = ../../irods/lib
DIR_WHEATIS_IRODS_INC = $(DIR_WHEATIS_IRODS)/include
DIR_WHEATIS_IRODS_LIB = $(DIR_WHEATIS_IRODS)/$(BUILD)
WHEATIS_IRODS_LIB_NAME = wheatis_irods

DIR_WHEATIS_HANDLER = ../../handlers/lib
DIR_WHEATIS_HANDLER_INC = $(DIR_WHEATIS_HANDLER)/include
DIR_WHEATIS_HANDLER_LIB = $(DIR_WHEATIS_HANDLER)/$(BUILD)
WHEATIS_HANDLER_LIB_NAME = wheatis_handler

DIR_WHEATIS_SERVER = ../lib
DIR_WHEATIS_SERVER_INC = $(DIR_WHEATIS_SERVER)/include
DIR_WHEATIS_SERVER_SRC = $(DIR_WHEATIS_SERVER)/src

DIR_ROOT = ../..

DIR_INSTALL = ../../../wheatis_demo/lib

ifeq ($(BUILD),release)
	CFLAGS 	+= -O3 -s
 	LDFLAGS += -s
else
#	CFLAGS 	+= -g
	CFLAGS 	+= -g -pg
	LDFLAGS += -pg
	CPPFLAGS += -D_DEBUG
endif

CPPFLAGS += -DUNIX

CFLAGS += -ansi -fpic

VPATH	= \
	$(DIR_SRC) \
	$(DIR_WHEATIS_SERVER_SRC) 	

INCLUDES = \
	-I ../include \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_SERVER_INC) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_NETWORK_INC) \
	-I$(DIR_WHEATIS_IRODS_INC) \
	-I$(DIR_WHEATIS_SERVICES_INC) \
	-I$(DIR_WHEATIS_HANDLER_INC) \
	-I$(DIR_IRODS)/lib/api/include \
	-I$(DIR_IRODS)/lib/core/include \
	-I$(DIR_IRODS)/lib/md5/include \
	-I$(DIR_IRODS)/lib/sha1/include \
	-I$(DIR_IRODS)/server/core/include \
	-I$(DIR_IRODS)/server/drivers/include \
	-I$(DIR_IRODS)/server/icat/include \
	-I$(DIR_IRODS)/server/re/include 	
		
SRCS 	= \
	src/mod_wheatis.c \
	src/key_value_pair.c \
	../lib/src/server.c \
 

LDFLAGS += -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) -L. -lirods

 
# Compile and generate dependency info
# 1. Compile the .c file
# 2. Generate dependency information, explicitly specifying the target name
$(DIR_OBJS)/%.o : %.c
	@echo ">>> c build for $@"
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	$(MAKEDEPEND) $(basename $@).d -MT $(basename $@).o $(CPPFLAGS) $(CFLAGS) $<  
	@mv -f $(basename $@).d $(basename $@).d.tmp
	@sed -e 's|.*:|$*.o:|' < $(basename $@).d.tmp > $(basename $@).d
	@sed -e 's/.*://' -e 's/\\$$//' < $(basename $@).d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $(basename $@).d
	@rm -f $(basename $@).d.tmp   	

.PHONY:	all

all: 
		apxs -n $(NAME) $(INCLUDES) -D UNIX=1 -D BUILD=$(BUILD) -c $(SRCS)
		
clean:
	@rm -fr src/*.o src/*.lo src/*.slo src/*.la
		
install: all
	@echo "Installing mod_$(NAME).so to $(DIR_APACHE_MODULES)"
	cp src/.libs/mod_$(NAME).so $(DIR_APACHE_MODULES)
		
		
