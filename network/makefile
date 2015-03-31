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

DIR_HTMLCXX = ../extras/htmlcxx
DIR_HTMLCXX_INC = $(DIR_HTMLCXX)/include
DIR_HTMLCXX_LIB = $(DIR_HTMLCXX)/lib

TEST_EXE_NAME = network_test

BUILD		= debug

VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_SERVER_INC) \
	-I$(DIR_HTMLCXX_INC) \
	-I$(DIR_JANSSON_INC)
	
	
SRCS 	= \
	byte_buffer.c \
	connection.c \
	curl_tools.c \
	json_tools.c \
	key_value_pair.c \
	request_tools.c \
	resource.c \
#	polar_ssl_connection.c \
	selector.cpp
	

BASE_LDFLAGS = -ldl -L$(DIR_JANSSON_LIB) -ljansson \
	-L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
#	-L/usr/local/lib/ -lpolarssl \
	-L/usr/lib/x86_64-linux-gnu/ -lcurl \
	-L../extras/hcxselect-1.1/src \
	-L $(DIR_HTMLCXX_LIB) \
	-lhcxselect \
	-lhtmlcxx \
	-Wl,-Bsymbolic-functions -Wl,-z,relro \
#	-lidn -llber -lldap -lrt -lgssapi_krb5 -lz -lnss3 -lnssutil3 -lsmime3 -lssl3 -lplds4 -lplc4 -lnspr4 -lrtmp

CPPFLAGS += -DWHEATIS_NETWORK_LIBRARY_EXPORTS -I/usr/include -I/usr/local/include -I../extras/hcxselect-1.1/src

LDFLAGS += $(BASE_LDFLAGS)


include $(DIR_ROOT)/generic_makefiles/shared_library.makefile

.PHONY:	test test_install


test:
	$(COMP)  -Wl,--no-as-needed -I/usr/include -L$(DIR_OBJS)/ -l$(NAME) -lm $(BASE_LDFLAGS) $(INCLUDES)  $(DIR_SRC)/test.c -g -o $(BUILD)/$(TEST_EXE_NAME)


test_install: test install
	@echo "Installing $(TEST_EXE_NAME) to $(DIR_INSTALL_ROOT)"
	cp $(BUILD)/$(TEST_EXE_NAME) $(DIR_INSTALL_ROOT)/  

