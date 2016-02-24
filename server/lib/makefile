NAME 		= grassroots_server
EXE_NAME = server
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

export DIR_INSTALL=$(DIR_GRASSROOTS_INSTALL)/lib

BUILD		= debug

VPATH	= \
	$(DIR_SRC) 
	
INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_GRASSROOTS_UTIL_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC)/containers \
	-I$(DIR_GRASSROOTS_UTIL_INC)/io \
	-I$(DIR_GRASSROOTS_NETWORK_INC) \
	-I$(DIR_GRASSROOTS_IRODS_INC) \
	-I$(DIR_GRASSROOTS_SERVICES_INC) \
	-I$(DIR_GRASSROOTS_HANDLER_INC) \
	-I$(DIR_GRASSROOTS_PARAMS_INC) \
	-I$(DIR_GRASSROOTS_DRMAA_INC) \
	-I$(DIR_SHARED_IRODS_INC)/lib/api/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/core/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/md5/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/sha1/include \
	-I$(DIR_SHARED_IRODS_INC)/server/core/include \
	-I$(DIR_SHARED_IRODS_INC)/server/drivers/include \
	-I$(DIR_SHARED_IRODS_INC)/server/icat/include \
	-I$(DIR_SHARED_IRODS_INC)/server/re/include \
	-I$(DIR_JANSSON_INC) \
	-I$(DIR_UUID_INC) \
	-I$(DIR_MONGODB_INC) \
	-I$(DIR_BSON_INC) \
	
SRCS 	= \
	jobs_manager.c \
	server.c \
	system_util.c \
	servers_pool.c \
	grassroots_config.c \
	mongodb_util.c

CPPFLAGS += -DGRASSROOTS_SERVICE_MANAGER_LIBRARY_EXPORTS 

LDFLAGS = -lpthread -L$(DIR_GRASSROOTS_UTIL_LIB) -l$(GRASSROOTS_UTIL_LIB_NAME) \
	-L$(DIR_JANSSON_LIB) -ljansson -lcurl \
	-L$(DIR_UUID_LIB) -luuid \
	-L$(DIR_GRASSROOTS_NETWORK_LIB) -l$(GRASSROOTS_NETWORK_LIB_NAME) \
	-L$(DIR_GRASSROOTS_SERVICES_LIB) -l$(GRASSROOTS_SERVICES_LIB_NAME) \
	-L$(DIR_GRASSROOTS_HANDLER_LIB) -l$(GRASSROOTS_HANDLER_LIB_NAME) \
	-L$(DIR_GRASSROOTS_PARAMS_LIB) -l$(GRASSROOTS_PARAMS_LIB_NAME) \
	-L$(DIR_MONGODB_LIB) -lmongoc-1.0
	

ifeq ($(DRMAA_ENABLED),1)
SRCS += drmaa_util.c
LDFLAGS += -L$(DIR_GRASSROOTS_DRMAA_LIB) -l$(GRASSROOTS_DRMAA_LIB_NAME)
INCLUDES += -I$(DIR_GRASSROOTS_DRMAA_INC)
CPPFLAGS += -DDRMAA_ENABLED=1
endif


include $(DIR_ROOT)/irods.mk

ifeq ($(IRODS_ENABLED), 1)

LDFLAGS += $(IRODS_EXTRA_LIBS)
endif

include $(DIR_ROOT)/generic_makefiles/shared_library.makefile	

