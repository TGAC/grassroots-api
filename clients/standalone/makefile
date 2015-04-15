NAME 		= client
DIR_SRC = src
DIR_INCLUDE = include
COMP		= g++
LINK		= ld
MAKEDEPEND 	= g++ -MM -MF


ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(../..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_ROOT)/project.properties
endif

export DIR_INSTALL=$(DIR_WHEATIS_INSTALL)/

BUILD		= debug

VPATH	= \
	$(DIR_SRC) 

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_NETWORK_INC) \
	-I$(DIR_WHEATIS_SERVER_INC) \
	-I$(DIR_WHEATIS_SERVICES_INC) \
	-I$(DIR_IRODS)/lib/core/include \
	-I$(DIR_JANSSON_INC)
	
SRCS 	= \
	standalone_client.c 

LDFLAGS += -L$(DIR_JANSSON_LIB) -ljansson \
	-L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME)  \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVICES_LIB) -l$(WHEATIS_SERVICES_LIB_NAME) \
	-L/usr/lib/x86_64-linux-gnu/ \
	-lcurl \
	-ldl \
	-L$(DIR_HCXSELECT_LIB) -lhcxselect \
	-L$(DIR_HTMLCXX_LIB) -lhtmlcxx

.PHONY:	all 

include $(DIR_ROOT)/generic_makefiles/executable.makefile


all: exe


install: all
	cp $(BUILD)/$(NAME) $(DIR_INSTALL)

