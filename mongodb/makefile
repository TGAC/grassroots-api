NAME 		= wheatis_mongodb
DIR_SRC = src
DIR_INCLUDE = include
COMP		= gcc
MAKEDEPEND 	= gcc -MM -MF



ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_ROOT)/project.properties
endif

export DIR_INSTALL=$(DIR_WHEATIS_INSTALL)/lib

BUILD		= debug

VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_MONGODB_INC) \
	-I$(DIR_BSON_INC)
	
SRCS 	= \
	mongodb_tool.c
	

BASE_LDFLAGS = -ldl \
	-L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME) \
	-L$(DIR_MONGODB_LIB) -lmongoc-1.0 \
	-L$(DIR_BSON_LIB) -lbson-1.0 
	

CPPFLAGS += -DWHEATIS_MONGODB_LIBRARY_EXPORTS

LDFLAGS += $(BASE_LDFLAGS)

include $(DIR_ROOT)/generic_makefiles/shared_library.makefile


