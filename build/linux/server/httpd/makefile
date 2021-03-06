SHELL = /bin/bash

NAME 		= grassroots
DIR_SRC = $(DIR_SERVER_SRC_ROOT)/httpd/mod_grassroots/src
DIR_INCLUDE = $(DIR_SERVER_SRC_ROOT)/httpd/mod_grassroots/include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF

DIR_OBJS = $(BUILD)

#DIR_APACHE=/opt/grassroots-0/apache


ifeq ($(DIR_APACHE),)
HTTPD := $(shell httpd -V) 
APXS := $(shell apxs)
DIR_APACHE := $(filter HTTPD_ROOT=%, $(HTTPD))
endif

ifneq ($(DIR_APACHE),)
DIR_APACHE:=$(subst HTTPD_ROOT=,,$(DIR_APACHE))
DIR_APACHE_MODULES := $(DIR_APACHE)/modules
DIR_APACHE_INC := $(DIR_APACHE)/include
DIR_APACHE_CONF_EXTRA = $(DIR_APACHE)/conf/extra
APACHE_SET = true

ifeq ($(APXS),)
APXS = $(DIR_APACHE)/bin/apxs
endif

endif


ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(../../..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_BUILD_ROOT)/project.properties
endif


CFLAGS 	+=  -Wc,-Wall  -Wc,-fpic

ifeq ($(BUILD),release)
	CFLAGS 	+= -Wc,-O3 -Wc,-s
 	LDFLAGS += Wl,-s
else
	CFLAGS 	+= -Wc,-g -Wc,-pg -Wc,-Wall -Wc,-O0
	LDFLAGS += -Wl,pg
	CPPFLAGS += -Wc,-D_DEBUG
endif


VPATH	= \
	$(DIR_SRC) \
	$(DIR_GRASSROOTS_SERVER_SRC) 	

INCLUDES = \
	-I ../include \
	-I$(DIR_APACHE_INC) \
	-I$(DIR_INCLUDE) \
	-I$(DIR_GRASSROOTS_SERVER_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC)/containers \
	-I$(DIR_GRASSROOTS_UTIL_INC)/io \
	-I$(DIR_GRASSROOTS_NETWORK_INC) \
	-I$(DIR_GRASSROOTS_IRODS_INC) \
	-I$(DIR_GRASSROOTS_SERVICES_INC) \
	-I$(DIR_GRASSROOTS_PARAMS_INC) \
	-I$(DIR_GRASSROOTS_HANDLER_INC) \
	-I$(DIR_JANSSON_INC) \
	-I$(DIR_UUID_INC)	
		
SRCS 	= \
	$(DIR_SRC)/mod_grassroots.c \
	$(DIR_SRC)/key_value_pair.c \
	$(DIR_SRC)/apr_global_storage.c \
	$(DIR_SRC)/apr_jobs_manager.c \
	$(DIR_SRC)/apr_external_servers_manager.c \
	$(DIR_SRC)/apache_output_stream.c

LDFLAGS += -L$(DIR_GRASSROOTS_UTIL_LIB) -l$(GRASSROOTS_UTIL_LIB_NAME) \
	-L$(DIR_GRASSROOTS_SERVICES_LIB) -l$(GRASSROOTS_SERVICES_LIB_NAME) \
	-L$(DIR_GRASSROOTS_HANDLER_LIB) -l$(GRASSROOTS_HANDLER_LIB_NAME) \
	-L$(DIR_GRASSROOTS_NETWORK_LIB) -l$(GRASSROOTS_NETWORK_LIB_NAME) \
	-L$(DIR_GRASSROOTS_SERVER_LIB) -l$(GRASSROOTS_SERVER_LIB_NAME) 



include $(DIR_BUILD_ROOT)/irods.mk
 
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
	@echo "APACHE_HOME  - $$APACHE_HOME -"
	@echo "DIR_APACHE_MODULES  $(DIR_APACHE_MODULES)"
	@echo "GRASSROOTS_INSTANCE -$(GRASSROOTS_INSTANCE)-"
	@echo "APXS $(APXS)"
	$(APXS) -n $(NAME) $(INCLUDES) -D UNIX=1 -D BUILD=$(BUILD) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -c $(SRCS)
		
clean:
	@rm -fr src/*.o src/*.lo src/*.slo src/*.la
		
install: all

	@echo "Installing mod_$(NAME).so to $(DIR_APACHE_MODULES)"
	$(SUDO) cp $(DIR_SRC)/.libs/mod_$(NAME).so $(DIR_APACHE_MODULES)
	#if [ -d $(DIR_APACHE)/conf/conf.d ]; then $(DIR_APACHE_CONF)=$(DIR_APACHE)/conf/conf.d; fi
	#$(SUDO) cp conf/extra/httpd-grassroots.conf $(DIR_APACHE_CONF)
	#$(SUDO) sed -i 's/DIR_GRASSROOTS_INSTALL/$(subst /,\/,$(DIR_GRASSROOTS_INSTALL))/g' $(DIR_APACHE_CONF)/httpd-grassroots.conf		
