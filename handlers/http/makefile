NAME 		= file_handler
DIR_SRC = src
DIR_INCLUDE = include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF


ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(../..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_ROOT)/project.properties
endif


export DIR_INSTALL=$(DIR_WHEATIS_INSTALL)/handlers


VPATH	= \
	$(DIR_SRC) 
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_HANDLE_INC) \
	-I$(DIR_WHEATIS_SERVICE_INC)

	
SRCS 	= \
	http_handler.c
	
CPPFLAGS = -DUNIX -DHTTP_HANDLER_EXPORTS

LDFLAGS += \
	-L$(DIR_WHEATIS_HANDLE_LIB) -l$(WHEATIS_HANDLE_LIB_NAME) \

#STATIC_LIBS += $(DIR_IRODS)/lib/core/obj/libRodsAPIs.a

include $(DIR_ROOT)/generic_makefiles/shared_library.makefile


