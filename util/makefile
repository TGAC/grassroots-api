NAME 		= wheatis_util
DIR_SRC = src
DIR_INCLUDE = include
COMP		= gcc
MAKEDEPEND 	= gcc -MM -MF

DIR_INSTALL = ../../wheatis_demo/lib

#DIR_IRODS	= /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1
DIR_IRODS	= $$IRODS_HOME

BUILD		= debug

VPATH	= \
	$(DIR_SRC) \
	$(DIR_SRC)/containers \
	$(DIR_SRC)/io \
	$(DIR_SRC)/platform \
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_INCLUDE)/containers \
	-I$(DIR_INCLUDE)/io \
	-I$(DIR_IRODS)/lib/api/include \
	-I$(DIR_IRODS)/lib/core/include \
	-I$(DIR_IRODS)/lib/md5/include \
	-I$(DIR_IRODS)/lib/sha1/include \
	-I$(DIR_IRODS)/server/core/include \
	-I$(DIR_IRODS)/server/drivers/include \
	-I$(DIR_IRODS)/server/icat/include \
	-I$(DIR_IRODS)/server/re/include \
	
	
SRCS 	= \
	filesystem_utils.c \
	io_utils.c  \
	json_util.c \
	linked_list.c \
	math_utils.c \
	parameter.c \
	parameter_set.c \
	plugin.c \
	service.c \
	services_manager.c \
	string_linked_list.c \
	string_utils.c \
	unix_filesystem.c \
	unix_plugin.c
	

ifeq ($(BUILD),release)
	CFLAGS 	+= -O3 -s
 	LDFLAGS += -s
else
#	CFLAGS 	+= -g
	CFLAGS 	+= -g -pg
	LDFLAGS += -pg
	CPPFLAGS += -D_DEBUG
endif
	

CPPFLAGS += -DWHEATIS_UTIL_LIBRARY_EXPORTS -DUNIX

LDFLAGS += -ldl -ljansson

#STATIC_LIBS += $(DIR_IRODS)/lib/core/obj/libRodsAPIs.a

include ../generic_makefiles/shared_library.makefile

.PHONY:	all test

all: lib test

test:
	$(COMP) $(CFLAGS) $(CPPFLAGS) -Wl,--no-as-needed -L$(DIR_OBJS)/ -l$(NAME) -lm  $(INCLUDES)  test.c -o test

