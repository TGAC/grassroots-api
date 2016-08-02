NAME 		= dropbox_handler
DIR_SRC = $(DIR_HANDLER_ROOT_SRC)/dropbox/src
DIR_INCLUDE = $(DIR_HANDLER_ROOT_SRC)/dropbox/include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF

ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(../..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_BUILD_ROOT)/project.properties
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_BUILD_ROOT)/project.properties
endif

export DIR_INSTALL=$(DIR_GRASSROOTS_INSTALL)/handlers

VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_GRASSROOTS_UTIL_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC)/containers \
	-I$(DIR_GRASSROOTS_UTIL_INC)/io \
	-I$(DIR_GRASSROOTS_HANDLER_INC) \
	-I$(DIR_GRASSROOTS_SERVER_INC) \
	-I$(DIR_GRASSROOTS_SERVICES_INC) \
	-I$(DIR_GRASSROOTS_NETWORK_INC) \
	-I$(DIR_GRASSROOTS_PARAMS_INC) \
	-I$(DIR_JANSSON_INC) \
	-I$(DIR_DROPBOX_INC) \
	-I$(DIR_UUID_INC)

SRCS 	= \
	dropbox_handler.c 
	
CPPFLAGS = -DDROPBOX_HANDLER_EXPORTS

LDFLAGS += \
	-L$(DIR_GRASSROOTS_HANDLE_LIB) -l$(GRASSROOTS_HANDLE_LIB_NAME) \
	-L$(DIR_GRASSROOTS_NETWORK_LIB) -l$(GRASSROOTS_NETWORK_LIB_NAME) \
	-L$(DIR_GRASSROOTS_UTIL_LIB) -l$(GRASSROOTS_UTIL_LIB_NAME) \
	-L$(DIR_DROPBOX_LIB) -ldropbox
	

include $(DIR_BUILD_ROOT)/generic_makefiles/shared_library.makefile

