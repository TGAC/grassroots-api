NAME 		= compress_service
DIR_SRC = src
DIR_INCLUDE = include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF

DIR_INSTALL = ../../../wheatis_demo/services

BUILD		= debug

DIR_IRODS	= $$IRODS_HOME

DIR_WHEATIS_UTIL = ../../util
DIR_WHEATIS_UTIL_INC = $(DIR_WHEATIS_UTIL)/include
DIR_WHEATIS_UTIL_LIB = $(DIR_WHEATIS_UTIL)/$(BUILD)
WHEATIS_UTIL_LIB_NAME = wheatis_util

DIR_WHEATIS_SERVICE = ../../services/lib
DIR_WHEATIS_SERVICE_INC = $(DIR_WHEATIS_SERVICE)/include
DIR_WHEATIS_SERVICE_LIB = $(DIR_WHEATIS_SERVICE)/$(BUILD)
WHEATIS_SERVICE_LIB_NAME = wheatis_service


ifeq ($(BUILD),release)
	CFLAGS 	+= -O3 -s
 	LDFLAGS += -s
else
#	CFLAGS 	+= -g
	CFLAGS 	+= -g -pg
	LDFLAGS += -pg
	CPPFLAGS += -D_DEBUG
endif


VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_SERVICE_INC) 
	
SRCS 	= \
	compress_service.c \
	zip.c
	
	
BASE_CPPFLAGS = -DUNIX 
CPPFLAGS += $(BASE_CPPFLAGS) -fPIC -DCOMPRESS_LIBRARY_EXPORTS 

BASE_LDFLAGS += -ljansson -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVICE_LIB) -l$(WHEATIS_SERVICE_LIB_NAME)

LDFLAGS += $(BASE_LDFLAGS)



#STATIC_LIBS += $(DIR_IRODS)/lib/core/obj/libRodsAPIs.a

include ../../generic_makefiles/shared_library.makefile

.PHONY:	all test exe

all: lib 


exe: all
	gcc $(BASE_CPPFLAGS) $(CFLAGS)-DUNIX -Wl,--no-as-needed -L$(DIR_OBJS)/ -l$(NAME) -lm -ldl -lz $(INCLUDES) $(EXE_OBJS) $(BASE_LDFLAGS)  src/unit_test.c  -o $(BUILD)/compress_test
	

