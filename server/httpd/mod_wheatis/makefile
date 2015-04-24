SHELL = /bin/bash

NAME 		= wheatis
DIR_SRC = src
DIR_INCLUDE = include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF

DIR_OBJS = $(BUILD)

HTTPD := $(shell httpd -V) 
DIR_APACHE := $(filter HTTPD_ROOT=%, $(HTTPD))

ifneq ($(DIR_APACHE),)
DIR_APACHE:=$(subst HTTPD_ROOT=,,$(DIR_APACHE))
DIR_APACHE_MODULES := $(DIR_APACHE)/modules
DIR_APACHE_INC := $(DIR_APACHE)/include
DIR_APACHE_CONF_EXTRA = $(DIR_APACHE)/conf/extra
APACHE_SET = true
endif



ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(../../..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_ROOT)/project.properties
endif


CFLAGS 	+=  -Wc,-Wall  -Wc,-fpic

ifeq ($(BUILD),release)
	CFLAGS 	+= -Wc,-O3 -Wc,-s
 	LDFLAGS += Wl,-s
gti pull#	CFLAGS 	+= -g
	CFLAGS 	+= -Wc,-g -Wc,-pg -Wc,-Wall -Wc,-O0
	LDFLAGS += -Wl,pg
	CPPFLAGS += -Wc,-D_DEBUG
endif


VPATH	= \
	$(DIR_SRC) \
	$(DIR_WHEATIS_SERVER_SRC) 	

INCLUDES = \
	-I ../include \
	-I$(DIR_APACHE_INC) \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_SERVER_INC) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_NETWORK_INC) \
	-I$(DIR_WHEATIS_IRODS_INC) \
	-I$(DIR_WHEATIS_SERVICES_INC) \
	-I$(DIR_WHEATIS_HANDLER_INC) \
	-I$(DIR_SHARED_IRODS_INC)/lib/api/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/core/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/md5/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/sha1/include \
	-I$(DIR_SHARED_IRODS_INC)/server/core/include \
	-I$(DIR_SHARED_IRODS_INC)/server/drivers/include \
	-I$(DIR_SHARED_IRODS_INC)/server/icat/include \
	-I$(DIR_SHARED_IRODS_INC)/server/re/include \
	-I$(DIR_JANSSON_INC)	
		
SRCS 	= \
	src/mod_wheatis.c \
	src/key_value_pair.c  

LDFLAGS += -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVICES_LIB) -l$(WHEATIS_SERVICES_LIB_NAME) \
	-L$(DIR_WHEATIS_HANDLER_LIB) -l$(WHEATIS_HANDLER_LIB_NAME) \
	-L$(DIR_WHEATIS_IRODS_LIB) -l$(WHEATIS_IRODS_LIB_NAME) \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVER_LIB) -l$(WHEATIS_SERVER_LIB_NAME) \
	-L$(SHARED_IRODS_HOME)/lib -lirods

 
# Compile and generate dependency info
# 1. Compile the .c file
# 2. Generate dependency information, explicitly specifying the target name
$(DIR_OBJS)/%.o : %.c
	@echo ">>> c build for $@"
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	$(MAKEDEPEND) $(basename $@).d -MT $(basename $@).o $(CPPFLAGS) $(CFLAGS) $<  
	@mv -f $(basename $@).d $(basename $@).d.tmp
	@sed -e 's|.*:|$*.o:|' < $(basename $@).d.tmp > $(basename $@).d
	@sed -e 's/.*://' -e 's/\\$$//' < $(basename $@).d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $(basename $@).d
	@rm -f $(basename $@).d.tmp   	

.PHONY:	all

all:
	@echo "HTTPD $(HTTPD)" 
	@echo "APACHE SET $(APACHE_SET)"
	@echo "APACHE_HOME  - $$APACHE_HOME -"
	@echo "IRODS_HOME $$IRODS_HOME -"
	@echo "DIR_APACHE _ $(DIR_APACHE) _"
	@echo "APACHE INC $(DIR_APACHE_INC)"
	apxs -n $(NAME) $(INCLUDES) -D UNIX=1 -D BUILD=$(BUILD) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -c $(SRCS)
		
clean:
	@rm -fr src/*.o src/*.lo src/*.slo src/*.la
		
install: all

	@echo "Installing mod_$(NAME).so to $(DIR_APACHE_MODULES)"
	$(SUDO) cp src/.libs/mod_$(NAME).so $(DIR_APACHE_MODULES)
	#if [ -d $(DIR_APACHE)/conf/conf.d ]; then $(DIR_APACHE_CONF)=$(DIR_APACHE)/conf/conf.d; fi
	#$(SUDO) cp conf/extra/httpd-wheatis.conf $(DIR_APACHE_CONF)
	#$(SUDO) sed -i 's/DIR_WHEATIS_INSTALL/$(subst /,\/,$(DIR_WHEATIS_INSTALL))/g' $(DIR_APACHE_CONF)/httpd-wheatis.conf		
