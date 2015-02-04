NAME 		= irods_search_service
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

DIR_WHEATIS_SERVICE = ../../services/lib
DIR_WHEATIS_SERVICE_INC = $(DIR_WHEATIS_SERVICE)/include
DIR_WHEATIS_SERVICE_LIB = $(DIR_WHEATIS_SERVICE)/$(BUILD)
WHEATIS_SERVICE_LIB_NAME = wheatis_service


DIR_WHEATIS_HANDLER = ../../handlers/lib
DIR_WHEATIS_HANDLER_INC = $(DIR_WHEATIS_HANDLER)/include
DIR_WHEATIS_HANDLER_LIB = $(DIR_WHEATIS_HANDLER)/$(BUILD)
WHEATIS_HANDLER_LIB_NAME = wheatis_handler


DIR_WHEATIS_IRODS = ../../irods/lib
DIR_WHEATIS_IRODS_INC = $(DIR_WHEATIS_IRODS)/include
DIR_WHEATIS_IRODS_LIB = $(DIR_WHEATIS_IRODS)/$(BUILD)
WHEATIS_IRODS_LIB_NAME = wheatis_irods


VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_SERVICE_INC) \
	-I$(DIR_WHEATIS_HANDLER_INC) \
	-I$(DIR_WHEATIS_IRODS_INC) \
	-I$(DIR_IRODS)/lib/api/include \
	-I$(DIR_IRODS)/lib/core/include \
	-I$(DIR_IRODS)/lib/md5/include \
	-I$(DIR_IRODS)/lib/sha1/include \
	-I$(DIR_IRODS)/server/core/include \
	-I$(DIR_IRODS)/server/drivers/include \
	-I$(DIR_IRODS)/server/icat/include \
	-I$(DIR_IRODS)/server/re/include 
	
SRCS 	= \
	irods_search_service.c 	
	 
CPPFLAGS += -DIRODS_SEARCH_LIBRARY_EXPORTS 

LDFLAGS += -ljansson -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVICE_LIB) -l$(WHEATIS_SERVICE_LIB_NAME) \
	-L$(DIR_WHEATIS_HANDLE_LIB) -l$(WHEATIS_HANDLE_LIB_NAME) \
	-L$(DIR_WHEATIS_IRODS_LIB) -l$(WHEATIS_IRODS_LIB_NAME) 


include $(DIR_ROOT)/generic_makefiles/shared_library.makefile


