NAME 		= client
DIR_SRC = src
DIR_INCLUDE = include
COMP		= g++
LINK		= ld
MAKEDEPEND 	= g++ -MM -MF

DIR_ROOT = ../..
DIR_IRODS	= $$IRODS_HOME
DIR_EXTERNAL_LIBS = ../../external_libs

DIR_WHEATIS_UTIL = ../../util
DIR_WHEATIS_UTIL_INC = $(DIR_WHEATIS_UTIL)/include
DIR_WHEATIS_UTIL_LIB = $(DIR_WHEATIS_UTIL)/$(BUILD)
WHEATIS_UTIL_LIB_NAME = wheatis_util


DIR_WHEATIS_NETWORK = ../../network
DIR_WHEATIS_NETWORK_INC = $(DIR_WHEATIS_NETWORK)/include
DIR_WHEATIS_NETWORK_LIB = $(DIR_WHEATIS_NETWORK)/$(BUILD)
WHEATIS_NETWORK_LIB_NAME = wheatis_network


DIR_WHEATIS_SERVICES = ../../services/lib
DIR_WHEATIS_SERVICES_INC = $(DIR_WHEATIS_SERVICES)/include
DIR_WHEATIS_SERVICES_LIB = $(DIR_WHEATIS_SERVICES)/$(BUILD)
WHEATIS_SERVICES_LIB_NAME = wheatis_service


DIR_WHEATIS_SERVER_INC = ../../server/lib/include


DIR_INSTALL = ../../../wheatis_demo

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
	-I$(DIR_IRODS)/lib/core/include 
	
SRCS 	= \
	standalone_client.c 

LDFLAGS += -ljansson \
	-L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME)  \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVICES_LIB) -l$(WHEATIS_SERVICES_LIB_NAME) \
	-L/usr/lib/x86_64-linux-gnu/ -lcurl\
	-ldl \
	-L$(DIR_EXTERNAL_LIBS) -l hcxselect

.PHONY:	all 

include $(DIR_ROOT)/generic_makefiles/executable.makefile


all: exe


install: all
	cp $(BUILD)/$(NAME) $(DIR_INSTALL)

