NAME 		= wheatis_handler
DIR_SRC = src
DIR_INCLUDE = include
COMP		= g++
LINK		= ld
MAKEDEPEND 	= g++ -MM -MF

DIR_ROOT = ../..

DIR_IRODS	= $$IRODS_HOME

DIR_INSTALL_ROOT = ../../../wheatis_demo
DIR_INSTALL = $(DIR_INSTALL_ROOT)/lib

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


BUILD		= debug

VPATH	= \
	$(DIR_SRC) 

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_NETWORK_INC) \
	-I$(DIR_WHEATIS_IRODS_INC) \
	-I$(DIR_WHEATIS_SERVICES_INC) \
	-I$(DIR_IRODS)/lib/api/include \
	-I$(DIR_IRODS)/lib/core/include \
	-I$(DIR_IRODS)/lib/md5/include \
	-I$(DIR_IRODS)/lib/sha1/include \
	-I$(DIR_IRODS)/server/core/include \
	-I$(DIR_IRODS)/server/drivers/include \
	-I$(DIR_IRODS)/server/icat/include \
	-I$(DIR_IRODS)/server/re/include 	
	
SRCS 	= \
	handler_utils.c \
	handler.c

CPPFLAGS += -DWHEATIS_HANDLELIBRARY_EXPORTS

LDFLAGS = -lpthread -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME)  -ljansson \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME) \
	-L$(DIR_WHEATIS_IRODS_LIB) -l$(WHEATIS_IRODS_LIB_NAME) \
#	-L$(DIR_WHEATIS_SERVICES_LIB) -l$(WHEATIS_SERVICES_LIB_NAME) \


include $(DIR_ROOT)/generic_makefiles/shared_library.makefile


