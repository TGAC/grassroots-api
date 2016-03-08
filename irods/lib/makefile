SHELL = /bin/bash

NAME 		= grassroots_irods
DIR_SRC = src
DIR_INCLUDE = include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF

BUILD		= debug


ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(../..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_ROOT)/project.properties
endif


export DIR_INSTALL=$(DIR_GRASSROOTS_INSTALL)/lib

CFLAGS += -ansi

VPATH	= \
	$(DIR_SRC) \

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_GRASSROOTS_UTIL_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC)/containers \
	-I$(DIR_GRASSROOTS_UTIL_INC)/io \
	-I$(DIR_GRASSROOTS_NETWORK_INC) \
	-I$(DIR_GRASSROOTS_SERVICES_INC) \
	-I$(DIR_JANSSON_INC) \
	-I$(DIR_UUID_INC)
	
SRCS 	= \
	connect.c \
	user.c \
	query.c \
	meta_search.c

CPPFLAGS += -DIRODS_UTIL_LIBRARY_EXPORTS

LDFLAGS += -L$(DIR_GRASSROOTS_UTIL_LIB) -l$(GRASSROOTS_UTIL_LIB_NAME) \
	-L$(DIR_GRASSROOTS_NETWORK_LIB) -l$(GRASSROOTS_NETWORK_LIB_NAME) \


include $(DIR_ROOT)/irods.mk

LDFLAGS += $(IRODS_DEP_LIBS)

include $(DIR_ROOT)/generic_makefiles/shared_library.makefile


.PHONY:	test

test: all
		$(CC) $(CFLAGS) $(CPPFLAGS) -Wl,--no-as-needed -L$(DIR_OBJS)/ -l$(NAME)  -lm -lpthread $(INCLUDES)  test.c -o test
