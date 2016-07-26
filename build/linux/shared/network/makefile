NAME 		= grassroots_network
DIR_SRC = $(DIR_GRASSROOTS_NETWORK)/src
DIR_INCLUDE = $(DIR_GRASSROOTS_NETWORK_INC)
COMP		= g++
MAKEDEPEND 	= gcc -MM -MF



ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_BUILD_ROOT)/project.properties
endif

export DIR_INSTALL=$(DIR_GRASSROOTS_INSTALL)/lib

TEST_EXE_NAME = network_test

BUILD		= debug

VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_GRASSROOTS_UTIL_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC)/containers \
	-I$(DIR_GRASSROOTS_UTIL_INC)/io \
	-I$(DIR_GRASSROOTS_SERVER_INC) \
	-I$(DIR_GRASSROOTS_SERVICES_INC) \
	-I$(DIR_HTMLCXX_INC) \
	-I$(DIR_JANSSON_INC) \
	-I$(DIR_UUID_INC)
	
	
SRCS 	= \
	byte_buffer.c \
	connection.c \
	curl_tools.c \
	json_tools.c \
	key_value_pair.c \
	request_tools.c \
	selector.cpp
	

BASE_LDFLAGS = -ldl -L$(DIR_JANSSON_LIB) -ljansson \
	-L$(DIR_GRASSROOTS_UTIL_LIB) -l$(GRASSROOTS_UTIL_LIB_NAME) \
	-L$(DIR_HCXSELECT_LIB) -lhcxselect \
	-L$(DIR_HTMLCXX_LIB) -lhtmlcxx 

CPPFLAGS += -DGRASSROOTS_NETWORK_LIBRARY_EXPORTS -I/usr/include -I/usr/local/include -I$(DIR_PKG_ROOT)/extras/hcxselect-1.1/src

LDFLAGS += $(BASE_LDFLAGS)


include $(DIR_BUILD_ROOT)/generic_makefiles/shared_library.makefile

.PHONY:	test test_install network

network: all


test:
	$(COMP)  -Wl,--no-as-needed -I/usr/include -L$(DIR_OBJS)/ -l$(NAME) -lm $(BASE_LDFLAGS) $(INCLUDES)  $(DIR_SRC)/test.c -g -o $(BUILD)/$(TEST_EXE_NAME)


test_install: test install
	@echo "Installing $(TEST_EXE_NAME) to $(DIR_INSTALL_ROOT)"
	cp $(BUILD)/$(TEST_EXE_NAME) $(DIR_INSTALL_ROOT)/  

