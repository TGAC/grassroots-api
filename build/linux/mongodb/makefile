NAME 		= grassroots_mongodb
DIR_SRC = $(DIR_SRC_ROOT)/mongodb/src
DIR_INCLUDE = $(DIR_SRC_ROOT)/mongodb/include
COMP		= gcc
MAKEDEPEND 	= gcc -MM -MF



ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_BUILD_ROOT)/project.properties
endif

export DIR_INSTALL=$(DIR_GRASSROOTS_INSTALL)/lib

BUILD		= debug

VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_GRASSROOTS_SERVER_INC) \
	-I$(DIR_GRASSROOTS_NETWORK_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC)/containers \
	-I$(DIR_GRASSROOTS_UTIL_INC)/io \
	-I$(DIR_MONGODB_INC) \
	-I$(DIR_UUID_INC) \
	-I$(DIR_BSON_INC) \
	-I$(DIR_JANSSON_INC)
	
SRCS 	= \
	mongodb_tool.c
	

BASE_LDFLAGS = -ldl \
	-L$(DIR_GRASSROOTS_UTIL_LIB) -l$(GRASSROOTS_UTIL_LIB_NAME) \
	-L$(DIR_GRASSROOTS_NETWORK_LIB) -l$(GRASSROOTS_NETWORK_LIB_NAME) \
	-L$(DIR_MONGODB_LIB) -lmongoc-1.0 \
	-L$(DIR_BSON_LIB) -lbson-1.0 
	

CPPFLAGS += -DGRASSROOTS_MONGODB_LIBRARY_EXPORTS

LDFLAGS += $(BASE_LDFLAGS)

include $(DIR_BUILD_ROOT)/generic_makefiles/shared_library.makefile


