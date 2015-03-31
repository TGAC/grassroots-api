NAME 		= wheatis_service
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
DIR_WHEATIS_NETWORKL_LIB = $(DIR_WHEATIS_NETWORK)/$(BUILD)
WHEATIS_NETWORK_LIB_NAME = wheatis_network

DIR_WHEATIS_HANDLER = ../../handlers/lib
DIR_WHEATIS_HANDLER_INC = $(DIR_WHEATIS_HANDLER)/include
DIR_WHEATIS_HANDLER_LIB = $(DIR_WHEATIS_HANDLER)/$(BUILD)
WHEATIS_HANDLER_LIB_NAME = wheatis_handler


BUILD		= debug

VPATH	= \
	$(DIR_SRC) \

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_HANDLER_INC) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_NETWORK_INC) \
	-I$(DIR_JANSSON_INC)
	
SRCS 	= \
	client.c \
	parameter.c \
	parameter_set.c \
	service.c \
	service_config.c \
	service_matcher.c \
	tag_item.c \
	user_details.c \
	web_service_util.c

CPPFLAGS += -DUNIX -DWHEATIS_SERVICE_LIBRARY_EXPORTS

BASE_LDFLAGS = -lpthread \
	-L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_WHEATIS_HANDLER_LIB) -l$(WHEATIS_HANDLER_LIB_NAME) \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME) \
	-L$(DIR_JANSSON_LIB) -ljansson 


LDFLAGS += $(BASE_LDFLAGS) 

ifeq ($(BUILD),release)
	CFLAGS 	+= -O3 -s
 	LDFLAGS += -s
else
#	CFLAGS 	+= -g
	CFLAGS 	+= -g -pg
	LDFLAGS += -pg
	CPPFLAGS += -D_DEBUG
endif
	

include $(DIR_ROOT)/generic_makefiles/shared_library.makefile




