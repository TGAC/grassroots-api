NAME 		= client
DIR_SRC = src
DIR_INCLUDE = include
COMP		= g++
LINK		= ld
MAKEDEPEND 	= g++ -MM -MF

DIR_IRODS	= $$IRODS_HOME

DIR_WHEATIS_UTIL = ../util
DIR_WHEATIS_UTIL_INC = $(DIR_WHEATIS_UTIL)/include
DIR_WHEATIS_UTIL_LIB = $(DIR_WHEATIS_UTIL)/$(BUILD)
WHEATIS_UTIL_LIB_NAME = wheatis_util

DIR_WHEATIS_NETWORK = ../network
DIR_WHEATIS_NETWORK_INC = $(DIR_WHEATIS_NETWORK)/include
DIR_WHEATIS_NETWORK_LIB = $(DIR_WHEATIS_NETWORK)/$(BUILD)
WHEATIS_NETWORK_LIB_NAME = wheatis_network

DIR_WHEATIS_SERVER_INC = ../server/include


DIR_INSTALL = ../../wheatis_demo

BUILD		= debug

VPATH	= \
	$(DIR_SRC) 

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_NETWORK_INC) \
	-I$(DIR_WHEATIS_SERVER_INC) \
	-I$(DIR_IRODS)/lib/core/include 
	
SRCS 	= \
	standalone_client.c 

CPPFLAGS += -DUNIX

LDFLAGS += -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME)  -ljansson -L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME)

ifeq ($(BUILD),release)
	CFLAGS 	+= -O3 -s
 	LDFLAGS += -s
else
#	CFLAGS 	+= -g
	CFLAGS 	+= -g -pg
	LDFLAGS += -pg
	CPPFLAGS += -D_DEBUG
endif


OBJS = $(patsubst %.c, $(BUILD)/%.o, $(SRCS))

.PHONY:	all init clean

all: init $(BUILD)/$(NAME)
	@echo "building $(OBJS)"


install: all
	cp $(BUILD)/$(NAME) $(DIR_INSTALL)

init:
	@mkdir -p $(BUILD)

clean:
	rm -f $(BUILD)/*


$(BUILD)/$(NAME) : $(OBJS)
	@echo "building exe $(BUILD)/$(NAME) from $(OBJS)"
	$(COMP) -o $@ $(OBJS) $(STATIC_LIBS) $(LDFLAGS)

$(BUILD)/%.o : %.c
	@echo "building objs  $(OBJS)"
	$(COMP) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -o $@ -c $<



