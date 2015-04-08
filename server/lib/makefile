NAME 		= wheatis_server
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


DIR_IRODS	= $$IRODS_HOME


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
	-I$(DIR_WHEATIS_HANDLER_INC) \
	-I$(DIR_IRODS)/lib/api/include \
	-I$(DIR_IRODS)/lib/core/include \
	-I$(DIR_IRODS)/lib/md5/include \
	-I$(DIR_IRODS)/lib/sha1/include \
	-I$(DIR_IRODS)/server/core/include \
	-I$(DIR_IRODS)/server/drivers/include \
	-I$(DIR_IRODS)/server/icat/include \
	-I$(DIR_IRODS)/server/re/include \
	-I$(DIR_JANSSON_INC)	
	
SRCS 	= \
	server.c \
	system_util.c

CPPFLAGS += -DWHEATIS_SERVICE_MANAGER_LIBRARY_EXPORTS 

LDFLAGS = -lpthread -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_JANSSON_LIB) -ljansson -lcurl \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME) \
	-L$(DIR_WHEATIS_IRODS_LIB) -l$(WHEATIS_IRODS_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVICES_LIB) -l$(WHEATIS_SERVICES_LIB_NAME) \
	-L$(DIR_WHEATIS_HANDLER_LIB) -l$(WHEATIS_HANDLER_LIB_NAME) \

	
include $(DIR_ROOT)/generic_makefiles/shared_library.makefile	

