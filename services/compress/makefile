NAME 		= compress_service
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
	-I$(DIR_JANSSON_INC)
	
SRCS 	= \
	compress_service.c \
	crc.c \
	gzip.c \
	zip.c
	
	 
CPPFLAGS += -DCOMPRESS_LIBRARY_EXPORTS 

LDFLAGS += -L$(DIR_JANSSON_LIB) -ljansson -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVICE_LIB) -l$(WHEATIS_SERVICE_LIB_NAME) \
	-L$(DIR_WHEATIS_HANDLE_LIB) -l$(WHEATIS_HANDLE_LIB_NAME) \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME) \
	-lz


include $(DIR_ROOT)/generic_makefiles/shared_library.makefile


