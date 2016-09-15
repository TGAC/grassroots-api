NAME 		= grassroots_service
DIR_SRC = $(DIR_GRASSROOTS_SERVICES)/src
DIR_INCLUDE = $(DIR_GRASSROOTS_SERVICES_INC)
COMP		= g++
LINK		= ld
MAKEDEPEND 	= g++ -MM -MF


ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(../..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_BUILD_ROOT)/project.properties
endif

export DIR_INSTALL=$(DIR_GRASSROOTS_INSTALL)/lib



BUILD		= debug

VPATH	= \
	$(DIR_SRC) \

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_GRASSROOTS_HANDLER_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC)/containers \
	-I$(DIR_GRASSROOTS_UTIL_INC)/io \
	-I$(DIR_GRASSROOTS_PARAMS_INC) \
	-I$(DIR_GRASSROOTS_NETWORK_INC) \
	-I$(DIR_GRASSROOTS_SERVER_INC) \
	-I$(DIR_JANSSON_INC) \
	-I$(DIR_GRASSROOTS_PARAMS_INC) \
	-I$(DIR_UUID_INC)
	
SRCS 	= \
	encode_dcc_metadata.c \
	linked_service.c \
	mapped_parameter.c \
	paired_service.c \
	provider.c \
	providers_state_table.c \
	remote_service_job.c \
	service.c \
	service_config.c \
	service_job.c \
	service_job_set_iterator.c \
	service_matcher.c \
	web_service_util.c

CPPFLAGS += -DUNIX -DGRASSROOTS_SERVICE_LIBRARY_EXPORTS

BASE_LDFLAGS = -lpthread \
	-L$(DIR_GRASSROOTS_UTIL_LIB) -l$(GRASSROOTS_UTIL_LIB_NAME) \
	-L$(DIR_GRASSROOTS_HANDLER_LIB) -l$(GRASSROOTS_HANDLER_LIB_NAME) \
	-L$(DIR_GRASSROOTS_NETWORK_LIB) -l$(GRASSROOTS_NETWORK_LIB_NAME) \
	-L$(DIR_JANSSON_LIB) -ljansson \
	-L$(DIR_UUID_LIB) -luuid

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
	

include $(DIR_BUILD_ROOT)/generic_makefiles/shared_library.makefile




