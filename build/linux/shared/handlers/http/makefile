NAME 		= http_handler
DIR_SRC = $(DIR_SRC_ROOT)/handlers/http/src
DIR_INCLUDE = $(DIR_SRC_ROOT)/handlers/http/include

CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF


ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(../..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_BUILD_ROOT)/project.properties
endif


export DIR_INSTALL=$(DIR_GRASSROOTS_INSTALL)/handlers


VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_GRASSROOTS_UTIL_INC) \
	-I$(DIR_GRASSROOTS_UTIL_INC)/containers \
	-I$(DIR_GRASSROOTS_UTIL_INC)/io \
	-I$(DIR_GRASSROOTS_HANDLE_INC) \
	-I$(DIR_GRASSROOTS_SERVICE_INC)

	
SRCS 	= \
	http_handler.c
	
CPPFLAGS = -DUNIX -DHTTP_HANDLER_EXPORTS

LDFLAGS += \
	-L$(DIR_GRASSROOTS_HANDLE_LIB) -l$(GRASSROOTS_HANDLE_LIB_NAME) \

#STATIC_LIBS += $(DIR_IRODS)/lib/core/obj/libRodsAPIs.a

include $(DIR_BUILD_ROOT)/generic_makefiles/shared_library.makefile


