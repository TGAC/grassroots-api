NAME 		= tgac_elastic_search_service
DIR_SRC = src
DIR_INCLUDE = include
CC		= gcc
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF

BUILD = debug

ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(../..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_ROOT)/project.properties
endif

export DIR_INSTALL=$(DIR_WHEATIS_INSTALL)/services

VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_NETWORK_INC) \
	-I$(DIR_WHEATIS_SERVICES_INC) \
	-I$(DIR_WHEATIS_HANDLER_INC) \
	-I$(DIR_JANSSON_INC) \
	-I$(DIR_UUID_INC)
	
SRCS 	= \
	tgac_elastic_search_service.c 
	 
CPPFLAGS += -DTGAC_ELASTIC_SEARCH_LIBRARY_EXPORTS 

LDFLAGS += -L$(DIR_JANSSON_LIB) -ljansson -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVICES_LIB) -l$(WHEATIS_SERVICES_LIB_NAME) \
	-L$(DIR_WHEATIS_HANDLER_LIB) -l$(WHEATIS_HANDLER_LIB_NAME) \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME) \
	-lcurl


include $(DIR_ROOT)/generic_makefiles/shared_library.makefile


