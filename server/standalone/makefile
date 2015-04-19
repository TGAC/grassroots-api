NAME = server
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
	-I$(DIR_WHEATIS_SERVER_INC) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_NETWORK_INC) \
	-I$(DIR_WHEATIS_IRODS_INC) \
	-I$(DIR_WHEATIS_SERVICES_INC) \
	-I$(DIR_WHEATIS_HANDLER_INC) \
	-I$(DIR_SHARED_IRODS_INC)/lib/api/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/core/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/md5/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/sha1/include \
	-I$(DIR_SHARED_IRODS_INC)/server/core/include \
	-I$(DIR_SHARED_IRODS_INC)/server/drivers/include \
	-I$(DIR_SHARED_IRODS_INC)/server/icat/include \
	-I$(DIR_SHARED_IRODS_INC)/server/re/include \
	-I$(DIR_JANSSON_INC) \
	-I$(DIR_UUID_INC)


SRCS = \
	standalone_server.c


CPPFLAGS += -DUNIX

LDFLAGS = -lpthread -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_JANSSON_LIB) -ljansson \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME) \
	-L$(DIR_WHEATIS_IRODS_LIB) -l$(WHEATIS_IRODS_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVICES_LIB) -l$(WHEATIS_SERVICES_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVER_LIB) -l$(WHEATIS_SERVER_LIB_NAME) \
	-L$(DIR_WHEATIS_HANDLER_LIB) -l$(WHEATIS_HANDLER_LIB_NAME) \
	-L$(DIR_SHARED_IRODS_LIB) -lirods \
	-L$(DIR_HCXSELECT_LIB) -lhcxselect \
	-L$(DIR_HTMLCXX_LIB) -lhtmlcxx \

.PHONY:	all 

include $(DIR_ROOT)/generic_makefiles/executable.makefile

all: exe

