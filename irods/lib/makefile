SHELL = /bin/bash

NAME 		= wheatis_irods
DIR_SRC = src
DIR_INCLUDE = include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF

BUILD		= debug

DIR_ROOT = ../..


DIR_IRODS = $$IRODS_HOME
#DIR_IRODS	= /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1


DIR_WHEATIS_SERVICES = ../../services/lib
DIR_WHEATIS_SERVICES_INC = $(DIR_WHEATIS_SERVICES)/include
DIR_WHEATIS_SERVICES_LIB = $(DIR_WHEATIS_SERVICES)/$(BUILD)
WHEATIS_SERVICES_LIB_NAME = wheatis_services

DIR_WHEATIS_UTIL = ../../util
DIR_WHEATIS_UTIL_INC = $(DIR_WHEATIS_UTIL)/include
DIR_WHEATIS_UTIL_LIB = $(DIR_WHEATIS_UTIL)/$(BUILD)
WHEATIS_UTIL_LIB_NAME = wheatis_util

DIR_WHEATIS_NETWORK = ../../network
DIR_WHEATIS_NETWORK_INC = $(DIR_WHEATIS_NETWORK)/include
DIR_WHEATIS_NETWORK_LIB = $(DIR_WHEATIS_NETWORK)/$(BUILD)
WHEATIS_NETWORK_LIB_NAME = wheatis_network


DIR_INSTALL = ../../../wheatis_demo/lib


CFLAGS += -ansi

VPATH	= \
	$(DIR_SRC) \
	$(DIR_IRODS)/lib/core/src \
	$(DIR_IRODS)/lib/api/src \

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_NETWORK_INC) \
	-I$(DIR_WHEATIS_SERVICES_INC) \
	-I$(DIR_IRODS)/lib/api/include \
	-I$(DIR_IRODS)/lib/core/include \
	-I$(DIR_IRODS)/lib/md5/include \
	-I$(DIR_IRODS)/lib/sha1/include \
	-I$(DIR_IRODS)/server/core/include \
	-I$(DIR_IRODS)/server/drivers/include \
	-I$(DIR_IRODS)/server/icat/include \
	-I$(DIR_IRODS)/server/re/include \
	-I$(DIR_JANSSON_INC)
	
SRCS 	= \
	connect.c \
	user.c \
	query.c \
	meta_search.c
#	procApiRequest.c \
#	rcSpecificQuery.c

CPPFLAGS += -DIRODS_HANDLER_EXPORTS

LDFLAGS += -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME) \
	-L$(DIR_IRODS_INSTALL) -lirods

#STATIC_LIBS += $(DIR_IRODS)/lib/core/obj/libRodsAPIs.a

include $(DIR_ROOT)/generic_makefiles/shared_library.makefile

.PHONY:	test

test: all
		$(CC) $(CFLAGS) $(CPPFLAGS) -Wl,--no-as-needed -L$(DIR_OBJS)/ -l$(NAME)  -lm -lpthread $(INCLUDES)  test.c -o test
