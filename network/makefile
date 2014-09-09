NAME 		= wheatis_network
DIR_SRC = src
DIR_INCLUDE = include
COMP		= gcc
MAKEDEPEND 	= gcc -MM -MF


DIR_WHEATIS_UTIL = ../util
DIR_WHEATIS_UTIL_INC = $(DIR_WHEATIS_UTIL)/include
DIR_WHEATIS_UTIL_LIB = $(DIR_WHEATIS_UTIL)/$(BUILD)
WHEATIS_UTIL_LIB_NAME = wheatis_util

DIR_INSTALL = ../../wheatis_demo/lib

BUILD		= debug

VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io 

	
	
	
SRCS 	= \
	json_tools.c \
	request_tools.c
	

ifeq ($(BUILD),release)
	CFLAGS 	+= -O3 -s
 	LDFLAGS += -s
else
#	CFLAGS 	+= -g
	CFLAGS 	+= -g -pg
	LDFLAGS += -pg
	CPPFLAGS += -D_DEBUG
endif
	

CPPFLAGS += -DWHEATIS_NETWORK_LIBRARY_EXPORTS -DUNIX

LDFLAGS += -ldl -ljansson -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME)

include ../generic_makefiles/shared_library.makefile

.PHONY:	all test

all: lib test

test:
	$(COMP) $(CFLAGS) $(CPPFLAGS) -Wl,--no-as-needed -L$(DIR_OBJS)/ -l$(NAME) -lm $(LDFLAGS) $(INCLUDES)  $(DIR_SRC)/test.c -o $(BUILD)/test

