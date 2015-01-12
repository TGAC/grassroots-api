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

DIR_INSTALL_ROOT = ../../wheatis_demo
DIR_INSTALL = $(DIR_INSTALL_ROOT)/lib

TEST_EXE_NAME = network_test

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
	curl_tools.c \
	json_tools.c \
	request_tools.c \
	polar_ssl_connection.c
	

BASE_LDFLAGS = -ldl -ljansson -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) -L/usr/local/lib/ -lpolarssl 	-L/usr/lib/x86_64-linux-gnu/ -lcurl 

CPPFLAGS += -DWHEATIS_NETWORK_LIBRARY_EXPORTS -I/usr/include

LDFLAGS += $(BASE_LDFLAGS)

include $(DIR_ROOT)/generic_makefiles/shared_library.makefile

.PHONY:	test test_install


test:
	$(COMP)  -Wl,--no-as-needed -I/usr/include -L$(DIR_OBJS)/ -l$(NAME) -lm $(BASE_LDFLAGS) $(INCLUDES)  $(DIR_SRC)/test.c -o $(BUILD)/$(TEST_EXE_NAME)


test_install: test install
	@echo "Installing $(TEST_EXE_NAME) to $(DIR_INSTALL_ROOT)"
	cp $(BUILD)/$(TEST_EXE_NAME) $(DIR_INSTALL_ROOT)/  

