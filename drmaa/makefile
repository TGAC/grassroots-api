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

TEST_EXE_NAME = network_test

BUILD		= debug

VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_LSF_DRMAA_INCLUDE)
	
SRCS 	= \
	lsf_example.c \
	drmaa_tool.c
	

BASE_LDFLAGS = -ldl -L$(DIR_JANSSON_LIB) -ljansson \
	-L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_LSF_DRMAA_LIB) -ldrmaa

CPPFLAGS += -DWHEATIS_DRMAA_LIBRARY_EXPORTS

LDFLAGS += $(BASE_LDFLAGS)


include $(DIR_ROOT)/generic_makefiles/shared_library.makefile

.PHONY:	test test_install


test:
	$(COMP)  -Wl,--no-as-needed -I/usr/include -L$(DIR_OBJS)/ -l$(NAME) -lm $(BASE_LDFLAGS) $(INCLUDES)  $(DIR_SRC)/test.c -g -o $(BUILD)/$(TEST_EXE_NAME)


test_install: test install
	@echo "Installing $(TEST_EXE_NAME) to $(DIR_INSTALL_ROOT)"
	cp $(BUILD)/$(TEST_EXE_NAME) $(DIR_INSTALL_ROOT)/  

