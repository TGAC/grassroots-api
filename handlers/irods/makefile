NAME 		= irods_handler
DIR_SRC = src
DIR_INCLUDE = include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF

DIR_INSTALL = ../../../wheatis_demo/handlers

BUILD		= debug

DIR_IRODS	= $$IRODS_HOME


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

DIR_WHEATIS_IRODS = ../../irods/lib
DIR_WHEATIS_IRODS_INC = $(DIR_WHEATIS_IRODS)/include
DIR_WHEATIS_IRODS_LIB = $(DIR_WHEATIS_IRODS)/$(BUILD)
WHEATIS_IRODS_LIB_NAME = wheatis_irods

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
	-I$(DIR_WHEATIS_HANDLE_INC) \
	-I$(DIR_WHEATIS_IRODS_INC) \
	-I$(DIR_WHEATIS_SERVICES_INC) \
	-I$(DIR_WHEATIS_NETWORK_INC) \
	-I$(DIR_IRODS)/lib/api/include \
	-I$(DIR_IRODS)/lib/core/include \
	-I$(DIR_IRODS)/lib/md5/include \
	-I$(DIR_IRODS)/lib/sha1/include \
	-I$(DIR_IRODS)/server/core/include \
	-I$(DIR_IRODS)/server/drivers/include \
	-I$(DIR_IRODS)/server/icat/include \
	-I$(DIR_IRODS)/server/re/include 
	
SRCS 	= \
	irods_handler.c \
	irods_resource.c
	
CPPFLAGS = -DUNIX -DIRODS_HANDLER_EXPORTS

LDFLAGS += \
	-L$(DIR_WHEATIS_HANDLE_LIB) -l$(WHEATIS_HANDLE_LIB_NAME) \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME) \
	-L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) 
	
	
#STATIC_LIBS += $(DIR_IRODS)/lib/core/obj/libRodsAPIs.a

include ../../generic_makefiles/shared_library.makefile

.PHONY:	all 

all: lib 

