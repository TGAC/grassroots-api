NAME 		= grassroots_drmaa_$(DRMAA_IMPLEMENTATION_NAME)
DIR_SRC = $(DIR_SERVER_SRC_ROOT)/drmaa/src
DIR_INCLUDE = $(DIR_SERVER_SRC_ROOT)/drmaa/include
COMP		= gcc
MAKEDEPEND 	= gcc -MM -MF



ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_BUILD_ROOT)/project.properties
endif

include $(DIR_BUILD_ROOT)/drmaa.properties

export DIR_INSTALL=$(DIR_GRASSROOTS_INSTALL)/lib

TEST_EXE_NAME = drmaa_tool_test_$(DRMAA_IMPLEMENTATION_NAME)


BUILD		= debug


VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_GRASSROOTS_SERVICES_INC) \
	-I$(DIR_GRASSROOTS_PARAMS_INC) \
	-I$(DIR_GRASSROOTS_NETWORK_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC)/io \
	-I$(DIR_GRASSROOTS_UTIL_INC)/containers \
	-I$(DIR_DRMAA_IMPLEMENTATION_INC) \
	-I$(DIR_UUID_INC) \
	-I$(DIR_JANSSON_INC)
	
	
SRCS 	= \
	drmaa_tool.c 

BASE_LDFLAGS = -ldl \
	-L$(DIR_GRASSROOTS_UTIL_LIB) -l$(GRASSROOTS_UTIL_LIB_NAME) \
	-L$(DIR_GRASSROOTS_NETWORK_LIB) -l$(GRASSROOTS_NETWORK_LIB_NAME) \
	-L$(DIR_GRASSROOTS_SERVICES_LIB) -l$(GRASSROOTS_SERVICES_LIB_NAME) \
	-L$(DIR_GRASSROOTS_PARAMS_LIB) -l$(GRASSROOTS_PARAMS_LIB_NAME) \
	-L$(DIR_JANSSON_LIB) -ljansson \
	-L$(DIR_DRMAA_IMPLEMENTATION_LIB) -l$(DRMAA_IMPLEMENTATION_LIB_NAME)

CPPFLAGS += -DGRASSROOTS_DRMAA_LIBRARY_EXPORTS

LDFLAGS += $(BASE_LDFLAGS)

EXE_LDFLAGS = -L$(DIR_GRASSROOTS_SERVER_LIB) -l$(GRASSROOTS_SERVER_LIB_NAME) \
	-L$(DIR_GRASSROOTS_SERVICES_LIB) -l$(GRASSROOTS_SERVICES_LIB_NAME) \
	-L$(DIR_GRASSROOTS_SERVER_LIB) -l$(GRASSROOTS_SERVER_LIB_NAME) \
	-L$(DIR_GRASSROOTS_NETWORK_LIB) -l$(GRASSROOTS_NETWORK_LIB_NAME) \
	-L$(DIR_GRASSROOTS_HANDLER_LIB) -l$(GRASSROOTS_HANDLER_LIB_NAME) \
	-L$(DIR_GRASSROOTS_IRODS_LIB) -l$(GRASSROOTS_IRODS_LIB_NAME) \
	-L$(DIR_HTMLCXX_LIB) -lhtmlcxx \
	-L$(DIR_HCXSELECT_LIB) -lhcxselect \
	-L$(DIR_SHARED_IRODS_LIB) -lirods

EXE_INCLUDES = \
	-I$(DIR_GRASSROOTS_SERVICES_INC) \
	-I$(DIR_GRASSROOTS_SERVER_INC)  \
	-I$(DIR_JANSSON_INC) \
	-I$(DIR_GRASSROOTS_HANDLER_INC)


ifeq ($(SLURM_DRMAA_ENABLED),1)
CPPFLAGS += -DSLURM_DRMAA_ENABLED
endif

ifeq ($(LSF_DRMAA_ENABLED),1)
CPPFLAGS += -DLSF_DRMAA_ENABLED
endif



include $(DIR_BUILD_ROOT)/generic_makefiles/shared_library.makefile

.PHONY:	test test_install

#install: test test_install

test:
	gcc $(CPPFLAGS) -Wl,--no-as-needed -I/usr/include $(EXE_INCLUDES) -L$(DIR_OBJS)/ -l$(NAME) -lm $(BASE_LDFLAGS) $(EXE_LDFLAGS) $(INCLUDES) ../clients/standalone/src/services_list.c ../clients/standalone/src/servers_manager.c \
	$(DIR_SRC)/$(TEST_EXE_NAME).c -g -o $(BUILD)/$(TEST_EXE_NAME)


test_install: test install
	@echo "Installing $(TEST_EXE_NAME) to $(DIR_GRASSROOTS_INSTALL)"
	cp $(BUILD)/$(TEST_EXE_NAME) $(DIR_GRASSROOTS_INSTALL)/  


