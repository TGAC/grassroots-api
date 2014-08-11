NAME 		= wheatis_qt_client
DIR_SRC = src
DIR_INCLUDE = include
COMP		= g++
MAKEDEPEND 	= g++ -MM -MF

DIR_INSTALL = ../modules/blast/lib/ 

#DIR_IRODS	= /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1
DIR_IRODS	= $$IRODS_HOME

#DIR_QT = /usr/users/ga002/tyrrells/Qt
DIR_QT = /home/billy/Qt
QT_VERSION = 5.3
DIR_QT_ROOT_INCLUDE = $(DIR_QT)/$(QT_VERSION)/gcc_64/include

#DIR_WHEATIS_UTIL = /usr/users/ga002/tyrrells/Projects/wheatis/irods/util
DIR_WHEATIS_UTIL = /home/billy/Projects/wheatis/irods/util




BUILD		= debug

VPATH	= \
	$(DIR_SRC) \
	$(DIR_SRC)/parameter_widgets \
	$(DIR_SRC)/prefs 

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_QT_ROOT_INCLUDE) \
	-I$(DIR_QT_ROOT_INCLUDE)/QtWidgets \
	-I$(DIR_QT_ROOT_INCLUDE)/QtCore \
	-I$(DIR_QT_ROOT_INCLUDE)/QtGui \
	-I$(DIR_WHEATIS_UTIL)/include \
	-I$(DIR_SRC)/prefs \
	-I$(DIR_SRC)/parameter_widgets
	
	
SRCS 	= \
	base_param_widget.cpp \
	file_chooser_widget.cpp \
	param_check_box.cpp \
	param_combo_box.cpp \
	param_double_spin_box.cpp \
	param_spin_box.cpp \
	param_text_box.cpp \
	prefs_widget.cpp \
	qt_parameter_widget.cpp \
	base_prefs_widget.cpp \
	prefs_window.cpp \
	gui.cpp


CPPFLAGS += -DWHEATIS_CLIENT_QT_LIBRARY_EXPORTS -DUNIX 

# Ubuntu build qt with -reduce-relocations so we need this flag
#CFLAGS += -fPIE

LDFLAGS +=

#STATIC_LIBS += $(DIR_IRODS)/lib/core/obj/libRodsAPIs.a

include ../../../generic_makefiles/shared_library.makefile

.PHONY:	all test

all: lib test

test:
	$(COMP) $(CPPFLAGS) -Wl,--no-as-needed -L$(DIR_OBJS)/ -l$(NAME) -lm  $(INCLUDES)  test.c -o test

