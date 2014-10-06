NAME 		= file_handler
DIR_SRC = src
DIR_INCLUDE = include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF

DIR_INSTALL = ../../../wheatis_demo/handlers

BUILD		= debug

DIR_IRODS	= $$IRODS_HOME


DIR_WHEATIS_UTIL = ../../util
DIR_WHEATIS_UTIL_INC = $(DIR_WHEATIS_UTIL)/include
DIR_WHEATIS_UTIL_LIB = $(DIR_WHEATIS_UTIL)/$(BUILD)
WHEATIS_UTIL_LIB_NAME = wheatis_util


DIR_WHEATIS_HANDLE = ../lib
DIR_WHEATIS_HANDLE_INC = $(DIR_WHEATIS_HANDLE)/include
DIR_WHEATIS_HANDLE_LIB = $(DIR_WHEATIS_HANDLE)/$(BUILD)
WHEATIS_HANDLE_LIB_NAME = wheatis_handler

ifeq ($(BUILD),release)
	CFLAGS 	+= -O3 -s
 	LDFLAGS += -s
else
#	CFLAGS 	+= -g
	CFLAGS 	+= -g -pg
	LDFLAGS += -pg
	CPPFLAGS += -D_DEBUG
endif


VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_HANDLE_INC) 
	
SRCS 	= \
	file_handler.c
	
CPPFLAGS = -DUNIX -DFILE_HANDLER_EXPORTS

LDFLAGS += \
	-L$(DIR_WHEATIS_HANDLE_LIB) -l$(WHEATIS_HANDLE_LIB_NAME) \

#STATIC_LIBS += $(DIR_IRODS)/lib/core/obj/libRodsAPIs.a

include ../../generic_makefiles/shared_library.makefile

.PHONY:	all 

all: lib 

