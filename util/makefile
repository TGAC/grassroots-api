NAME 		= grassroots_util
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

export DIR_INSTALL=$(DIR_GRASSROOTS_INSTALL)/lib


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
	-I$(DIR_SHARED_IRODS_INC)/lib/api/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/core/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/md5/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/sha1/include \
	-I$(DIR_SHARED_IRODS_INC)/server/core/include \
	-I$(DIR_SHARED_IRODS_INC)/server/drivers/include \
	-I$(DIR_SHARED_IRODS_INC)/server/icat/include \
	-I$(DIR_SHARED_IRODS_INC)/server/re/include \
	-I$(DIR_JANSSON_INC) \
	-I$(DIR_UUID_INC)
	
SRCS 	= \
	file_output_stream.c \
	filesystem_utils.c \
	hash_table.c \
	io_utils.c  \
	json_util.c \
	linked_list.c \
	linked_list_iterator.c \
	math_utils.c \
	operation.c \
	plugin.c \
	streams.c \
	string_hash_table.c \
	string_int_pair.c \
	string_linked_list.c \
	string_utils.c \
	time_util.c \
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
	

CPPFLAGS += -DGRASSROOTS_UTIL_LIBRARY_EXPORTS -DUNIX 

LDFLAGS += -ldl \
	-L$(DIR_JANSSON_LIB) -ljansson \
	-L$(DIR_UUID_LIB) -luuid

#STATIC_LIBS += $(DIR_IRODS)/lib/core/obj/libRodsAPIs.a

include $(DIR_ROOT)/generic_makefiles/shared_library.makefile

.PHONY:	util all test


util: all

test:
	$(COMP) $(CFLAGS) $(CPPFLAGS) -Wl,--no-as-needed -L$(DIR_OBJS)/ -l$(NAME) -lm  $(INCLUDES)  test.c -o test

