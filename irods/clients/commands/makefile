NAME 		= wheatis_commands
DIR_SRC = src
DIR_INCLUDE = include
COMP		= g++
LINK		= ld
MAKEDEPEND 	= g++ -MM -MF

DIR_IRODS	= $$IRODS_HOME


DIR_WHEATIS_UTIL = /usr/users/ga002/tyrrells/Projects/wheatis/irods/util
#DIR_WHEATIS_UTIL = /home/billy/Projects/wheatis/irods/util


BUILD		= debug

VPATH	= \
	$(DIR_SRC) 

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL)/include \
	-I$(DIR_WHEATIS_UTIL)/include/containers \
	-I$(DIR_IRODS)/lib/api/include \
	-I$(DIR_IRODS)/lib/core/include \
	-I$(DIR_IRODS)/lib/md5/include \
	-I$(DIR_IRODS)/lib/sha1/include \
	-I$(DIR_IRODS)/server/core/include \
	-I$(DIR_IRODS)/server/drivers/include \
	-I$(DIR_IRODS)/server/icat/include \
	-I$(DIR_IRODS)/server/re/include 
	
SRCS 	= \
	tgac_put.c 

CPPFLAGS += -DUNIX

# Ubuntu build qt with -reduce-relocations so we need this flag
#CFLAGS += -fPIE

LDFLAGS += -lpthread -L/usr/users/ga002/tyrrells/Projects/wheatis_demo/lib -lwheatis_util

OBJS = $(patsubst %.c,%.o, $(SRCS))
EXES = $(patsubst %.c,%, $(SRCS))


STATIC_LIBS += $(DIR_IRODS)/lib/core/obj/libRodsAPIs.a 


.PHONY:	all init clean

all: init $(BUILD)/$(EXES)
	@echo "building $(BUILD)/$(EXES)"
	@echo "building $(BUILD)/$(OBJS)"


init:
	@mkdir -p $(BUILD)

clean:
	rm -f $(BUILD)/*

$(BUILD)/%.o : %.c
	@echo "building objs $(BUILD)/$(OBJS)"
	$(COMP) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -o $@ -c $<


$(BUILD)/% : $(BUILD)/%.o
	@echo "building objs $(BUILD)/$(EXES)"
	$(COMP) -o $@ $< $(STATIC_LIBS) $(LDFLAGS)
