NAME 		= wheatis_server
EXE_NAME = server
DIR_SRC = src
DIR_INCLUDE = include
COMP		= g++
LINK		= ld
MAKEDEPEND 	= g++ -MM -MF


DIR_INSTALL_ROOT = ../../wheatis_demo
DIR_INSTALL = $(DIR_INSTALL_ROOT)/lib

DIR_WHEATIS_UTIL = ../util
DIR_WHEATIS_UTIL_INC = $(DIR_WHEATIS_UTIL)/include
DIR_WHEATIS_UTIL_LIB = $(DIR_WHEATIS_UTIL)/$(BUILD)
WHEATIS_UTIL_LIB_NAME = wheatis_util

DIR_WHEATIS_NETWORK = ../network
DIR_WHEATIS_NETWORK_INC = $(DIR_WHEATIS_NETWORK)/include
DIR_WHEATIS_NETWORK_LIB = $(DIR_WHEATIS_NETWORK)/$(BUILD)
WHEATIS_NETWORK_LIB_NAME = wheatis_network


DIR_CTHREAD_POOL_DEP = deps/c_thread_pool

BUILD		= debug

VPATH	= \
	$(DIR_SRC) \
	$(DIR_CTHREAD_POOL_DEP)

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_CTHREAD_POOL_DEP) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_NETWORK_INC)
	
SRCS 	= \
	parameter.c \
	parameter_set.c \
	plugin.c \
	server.c \
	service.c \
	services_manager.c \
	unix_plugin.c

EXE_SRCS = \
	standalone_server.c \
	thpool.c


EXE_OBJS = $(patsubst %.c, $(DIR_OBJS)/%.o, $(EXE_SRCS))

CPPFLAGS += -DUNIX -DWHEATIS_SERVICE_MANAGER_LIBRARY_EXPORTS -DSHARED_LIBRARY

BASE_LDFLAGS = -lpthread -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME)  -ljansson -L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME)


LDFLAGS += $(BASE_LDFLAGS)

ifeq ($(BUILD),release)
	CFLAGS 	+= -O3 -s
 	LDFLAGS += -s
else
#	CFLAGS 	+= -g
	CFLAGS 	+= -g -pg
	LDFLAGS += -pg
	CPPFLAGS += -D_DEBUG
endif
	

ifeq ($(BUILD),release)
	CFLAGS 	+= -O3 -s
 	LDFLAGS += -s
else
#	CFLAGS 	+= -g
	CFLAGS 	+= -g -pg
	LDFLAGS += -pg
	CPPFLAGS += -D_DEBUG
endif

include ../generic_makefiles/shared_library.makefile

.PHONY:	all exe


install_exe: all
	chmod u+x $(BUILD)/$(EXE_NAME)
	cp $(BUILD)/$(EXE_NAME) $(DIR_INSTALL_ROOT)

all: lib exe

exe: lib $(EXE_OBJS)
	$(COMP) $(CFLAGS) -DUNIX -Wl,--no-as-needed -L$(DIR_OBJS)/ -l$(NAME) -lm -ldl $(INCLUDES) $(EXE_OBJS) $(BASE_LDFLAGS) -o $(BUILD)/$(EXE_NAME)



