NAME 		= tgac_irods_util
DIR_SRC = src
DIR_INCLUDE = include
COMP		= gcc
MAKEDEPEND 	= gcc -MM -MF

DIR_INSTALL = ../modules/blast/lib/ 

#DIR_IRODS	= /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1
DIR_IRODS	= $$IRODS_HOME

BUILD		= debug

VPATH	= \
	$(DIR_SRC)

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_IRODS)/lib/api/include \
	-I$(DIR_IRODS)/lib/core/include \
	-I$(DIR_IRODS)/lib/md5/include \
	-I$(DIR_IRODS)/lib/sha1/include \
	-I$(DIR_IRODS)/server/core/include \
	-I$(DIR_IRODS)/server/drivers/include \
	-I$(DIR_IRODS)/server/icat/include \
	-I$(DIR_IRODS)/server/re/include \
	
	
SRCS 	= \
	io_utils.c  

CPPFLAGS += -DIRODS_LIBRARY_EXPORTS

LDFLAGS += 

include shared_library.makefile

.PHONY:	all test

all: lib test

test:
	$(COMP) -Wl,--no-as-needed -L$(DIR_OBJS)/ -l$(NAME) -lm  $(INCLUDES)  test.c -o test

