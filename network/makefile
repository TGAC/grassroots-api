NAME 		= wheatis_network
DIR_SRC = src
DIR_INCLUDE = include
COMP		= gcc
MAKEDEPEND 	= gcc -MM -MF


DIR_WHEATIS_UTIL = ../util
DIR_WHEATIS_UTIL_INC = $(DIR_WHEATIS_UTIL)/include
DIR_WHEATIS_UTIL_LIB = $(DIR_WHEATIS_UTIL)/$(BUILD)
WHEATIS_UTIL_LIB_NAME = wheatis_util

DIR_WHEATIS_SERVER = ../server/lib
DIR_WHEATIS_SERVER_INC = $(DIR_WHEATIS_SERVER)/include

DIR_ROOT = ..

DIR_INSTALL = ../../wheatis_demo/lib

BUILD		= debug

VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_SERVER_INC)

	
	
	
SRCS 	= \
	byte_buffer.c \
	json_tools.c \
	request_tools.c \
	polar_ssl_connection.c
	


CPPFLAGS += -DWHEATIS_NETWORK_LIBRARY_EXPORTS

LDFLAGS += -ldl -ljansson -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) -L/usr/local/lib/ -lpolarssl

include $(DIR_ROOT)/generic_makefiles/shared_library.makefile

.PHONY:	test


test:
	$(COMP) $(CFLAGS) $(CPPFLAGS) -Wl,--no-as-needed -L$(DIR_OBJS)/ -l$(NAME) -lm $(LDFLAGS) $(INCLUDES)  $(DIR_SRC)/test.c -o $(BUILD)/test

