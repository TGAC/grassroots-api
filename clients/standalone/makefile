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

export DIR_INSTALL=$(DIR_GRASSROOTS_INSTALL)/

BUILD		= debug

VPATH	= \
	$(DIR_SRC) 

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_GRASSROOTS_UTIL_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC)/containers \
	-I$(DIR_GRASSROOTS_UTIL_INC)/io \
	-I$(DIR_GRASSROOTS_NETWORK_INC) \
	-I$(DIR_GRASSROOTS_SERVER_INC) \
	-I$(DIR_GRASSROOTS_SERVICES_INC) \
	-I$(DIR_GRASSROOTS_HANDLER_INC) \
	-I$(DIR_GRASSROOTS_CLIENTS_INC) \
	-I$(DIR_GRASSROOTS_PARAMS_INC) \
	-I$(DIR_SHARED_IRODS_INC)/lib/core/include \
	-I$(DIR_JANSSON_INC) \
	-I$(DIR_UUID_INC)
	
SRCS 	= \
	standalone_client.c \
#	services_list.c

LDFLAGS += -L$(DIR_JANSSON_LIB) -ljansson \
	-L$(DIR_GRASSROOTS_UTIL_LIB) -l$(GRASSROOTS_UTIL_LIB_NAME)  \
	-L$(DIR_GRASSROOTS_NETWORK_LIB) -l$(GRASSROOTS_NETWORK_LIB_NAME) \
	-L$(DIR_GRASSROOTS_PARAMS_LIB) -l$(GRASSROOTS_PARAMS_LIB_NAME) \
	-L$(DIR_GRASSROOTS_CLIENTS_LIB) -l$(GRASSROOTS_CLIENTS_LIB_NAME) \
	-L/usr/lib/x86_64-linux-gnu/ \
	-lcurl \
	-ldl \
	-L$(DIR_HCXSELECT_LIB) -lhcxselect \
	-L$(DIR_HTMLCXX_LIB) -lhtmlcxx

.PHONY:	all 


#ifneq ($(DRMAA_ENABLED),)
#LDFLAGS += -L$(DIR_GRASSROOTS_DRMAA_LIB) -l$(GRASSROOTS_DRMAA_LIB_NAME) -L$(DIR_LSF_DRMAA_LIB) -ldrmaa
#INCLUDES += -I$(DIR_LSF_DRMAA_INC) -I$(DIR_GRASSROOTS_DRMAA_INC)
#CPPFLAGS += -DDRMAA_ENABLED=1
#endif



include $(DIR_ROOT)/generic_makefiles/executable.makefile


all: exe


install: all
	cp $(BUILD)/$(NAME) $(DIR_INSTALL)
