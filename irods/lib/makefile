SHELL = /bin/bash

NAME 		= wheatis_irods
DIR_SRC = src
DIR_INCLUDE = include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF

BUILD		= debug

#DIR_IRODS = $$IRODS_HOME
DIR_IRODS	= /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1

DIR_WHEATIS_UTIL = ../../util
DIR_WHEATIS_UTIL_INC = $(DIR_WHEATIS_UTIL)/include
DIR_WHEATIS_UTIL_LIB = $(DIR_WHEATIS_UTIL)/$(BUILD)
WHEATIS_UTIL_LIB_NAME = wheatis_util

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


CFLAGS += -ansi

VPATH	= \
	$(DIR_SRC) \
	$(DIR_IRODS)/lib/core/src \
	$(DIR_IRODS)/lib/api/src \

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_IRODS)/lib/api/include \
	-I$(DIR_IRODS)/lib/core/include \
	-I$(DIR_IRODS)/lib/md5/include \
	-I$(DIR_IRODS)/lib/sha1/include \
	-I$(DIR_IRODS)/server/core/include \
	-I$(DIR_IRODS)/server/drivers/include \
	-I$(DIR_IRODS)/server/icat/include \
	-I$(DIR_IRODS)/server/re/include 
	
SRCS 	= \
	connect.c \
	io.c \
	user.c \
	query.c \
	procApiRequest.c \
	rcSpecificQuery.c

CPPFLAGS += -DIRODS_UTIL_LIBRARY_EXPORTS -DUNIX -fPIC

LDFLAGS += -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) -L. -lirods

#STATIC_LIBS += $(DIR_IRODS)/lib/core/obj/libRodsAPIs.a

include ../../generic_makefiles/shared_library.makefile

.PHONY:	all test

all: lib 


test: all
		$(CC) $(CFLAGS) $(CPPFLAGS) -Wl,--no-as-needed -L$(DIR_OBJS)/ -l$(NAME)  -lm -lpthread $(INCLUDES)  test.c -o test
