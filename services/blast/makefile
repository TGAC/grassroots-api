NAME 		= blast_service
DIR_SRC = src
DIR_INCLUDE = include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF

DIR_ROOT = ../..
DIR_INSTALL = ../../../wheatis_demo/services

BUILD		= debug

DIR_IRODS	= $$IRODS_HOME

DIR_WHEATIS_UTIL = ../../util
DIR_WHEATIS_UTIL_INC = $(DIR_WHEATIS_UTIL)/include
DIR_WHEATIS_UTIL_LIB = $(DIR_WHEATIS_UTIL)/$(BUILD)
WHEATIS_UTIL_LIB_NAME = wheatis_util

DIR_WHEATIS_SERVICE = ../lib
DIR_WHEATIS_SERVICE_INC = $(DIR_WHEATIS_SERVICE)/include
DIR_WHEATIS_SERVICE_LIB = $(DIR_WHEATIS_SERVICE)/$(BUILD)
WHEATIS_SERVICE_LIB_NAME = wheatis_service

DIR_WHEATIS_NETWORK = ../../network
DIR_WHEATIS_NETWORK_INC = $(DIR_WHEATIS_NETWORK)/include
DIR_WHEATIS_NETWORK_LIB = $(DIR_WHEATIS_NETWORK)/$(BUILD)
WHEATIS_NETWORK_LIB_NAME = wheatis_network


DIR_WHEATIS_HANDLER = ../../handlers/lib
DIR_WHEATIS_HANDLER_INC = $(DIR_WHEATIS_HANDLER)/include
DIR_WHEATIS_HANDLER_LIB = $(DIR_WHEATIS_HANDLER)/$(BUILD)
WHEATIS_HANDLER_LIB_NAME = wheatis_handler


VPATH	= \
	$(DIR_SRC) \
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_HANDLER_INC) \
	-I$(DIR_WHEATIS_NETWORK_INC) \
	-I$(DIR_WHEATIS_SERVICE_INC) \
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
	blast_service.c \
	blast_tool.cpp 
	
	

CPPFLAGS += -DBLAST_LIBRARY_EXPORTS 

LDFLAGS += -L$(DIR_JANSSON_LIB) -ljansson -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVICE_LIB) -l$(WHEATIS_SERVICE_LIB_NAME) \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME)


include $(DIR_ROOT)/generic_makefiles/shared_library.makefile

