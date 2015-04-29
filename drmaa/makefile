NAME 		= wheatis_drmaa
DIR_SRC = src
DIR_INCLUDE = include
COMP		= gcc
MAKEDEPEND 	= gcc -MM -MF



ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_ROOT)/project.properties
endif

export DIR_INSTALL=$(DIR_WHEATIS_INSTALL)/lib

TEST_EXE_NAME = drmaa_tool_test

BUILD		= debug

VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_LSF_DRMAA_INC) \
	-I$(DIR_UUID_INC) \
	-I$(DIR_WHEATIS_SERVER_INC) \
	-I/tgac/common/lsf/9.1.1/include/
	
SRCS 	= \
	drmaa_tool.c
	

BASE_LDFLAGS = -ldl \
	-L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_JANSSON_LIB) -ljansson \
	-L$(DIR_LSF_DRMAA_LIB) -ldrmaa

CPPFLAGS += -DWHEATIS_DRMAA_LIBRARY_EXPORTS

LDFLAGS += $(BASE_LDFLAGS)

EXE_LDFLAGS = -L$(DIR_WHEATIS_SERVER_LIB) -l$(WHEATIS_SERVER_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVICES_LIB) -l$(WHEATIS_SERVICES_LIB_NAME) \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME)\
	-L$(DIR_WHEATIS_HANDLER_LIB) -l$(WHEATIS_HANDLER_LIB_NAME) \
	-L$(DIR_WHEATIS_IRODS_LIB) -l$(WHEATIS_IRODS_LIB_NAME) \
	-L$(DIR_HTMLCXX_LIB) -lhtmlcxx \
	-L$(DIR_HCXSELECT_LIB) -lhcxselect \
	-L$(DIR_SHARED_IRODS_LIB) -lirods

include $(DIR_ROOT)/generic_makefiles/shared_library.makefile

.PHONY:	test test_install

install: test test_install

test:
	gcc $(CPPFLAGS) -Wl,--no-as-needed -I/usr/include -L$(DIR_OBJS)/ -l$(NAME) -lm $(BASE_LDFLAGS) $(EXE_LDFLAGS) $(INCLUDES)  $(DIR_SRC)/$(TEST_EXE_NAME).c -g -o $(BUILD)/$(TEST_EXE_NAME)


test_install: test install
	@echo "Installing $(TEST_EXE_NAME) to $(DIR_WHEATIS_INSTALL)"
	cp $(BUILD)/$(TEST_EXE_NAME) $(DIR_WHEATIS_INSTALL)/  


