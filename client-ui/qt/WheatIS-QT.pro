#-------------------------------------------------
#
# Project created by QtCreator 2014-09-11T12:08:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wheatis-qt-client
TEMPLATE = lib

VPATH += src include

unix {
 CONFIG(release, debug|release) {
	BUILD = release
 }
 CONFIG(debug, debug|release) {
	BUILD = debug
 }
}


DIR_WHEATIS = /usr/users/ga002/tyrrells/Projects/wheatis

DIR_WHEATIS_UTIL = $$DIR_WHEATIS/util
DIR_WHEATIS_UTIL_INC = $$DIR_WHEATIS_UTIL/include
DIR_WHEATIS_UTIL_LIB = $$DIR_WHEATIS_UTIL/$$BUILD
WHEATIS_UTIL_LIB_NAME = wheatis_util

DIR_WHEATIS_SERVER = $$DIR_WHEATIS/server
DIR_WHEATIS_SERVER_INC = $$DIR_WHEATIS_SERVER/include
DIR_WHEATIS_SERVER_LIB = $$DIR_WHEATIS_SERVER/$$BUILD
WHEATIS_SERVER_LIB_NAME = wheatis_server


DIR_WHEATIS_UTIL = $$DIR_WHEATIS/util
DIR_WHEATIS_UTIL_INC = $$DIR_WHEATIS_UTIL/include
DIR_WHEATIS_UTIL_LIB = $$DIR_WHEATIS_UTIL/$$BUILD
WHEATIS_UTIL_LIB_NAME = wheatis_util

DIR_WHEATIS_SERVICE = $$DIR_WHEATIS/services/lib
DIR_WHEATIS_SERVICE_INC = $$DIR_WHEATIS_SERVICE/include
DIR_WHEATIS_SERVICE_LIB = $$DIR_WHEATIS_SERVICE/$$BUILD
WHEATIS_SERVICE_LIB_NAME = wheatis_service


DEFINES += "UNIX"

INCLUDEPATH += include \
	$$DIR_WHEATIS_UTIL_INC \
	$$DIR_WHEATIS_UTIL_INC/containers \
	$$DIR_WHEATIS_UTIL_INC/io \
	$$DIR_WHEATIS_SERVICE_INC \


SOURCES += \
    src/base_param_widget.cpp \
    src/file_chooser_widget.cpp \
    src/param_check_box.cpp \
    src/param_combo_box.cpp \
    src/param_double_spin_box.cpp \
    src/param_spin_box.cpp \
    src/param_text_box.cpp \
    src/prefs_widget.cpp \
    src/qt_parameter_widget.cpp \
    src/client_ui_api.cpp

HEADERS  += \
    include/wheatis_ui.h \
    include/base_param_widget.h \
    include/file_chooser_widget.h \
    include/param_check_box.h \
    include/param_combo_box.h \
    include/param_double_spin_box.h \
    include/param_spin_box.h \
    include/param_text_box.h \
    include/prefs_widget.h \
    include/qt_parameter_widget.h \
    include/client_ui_api.h

LIBS += -L$$DIR_WHEATIS_UTIL_LIB  -l$$WHEATIS_UTIL_LIB_NAME \
 -L$$DIR_WHEATIS_SERVICE_LIB  -l$$WHEATIS_SERVICE_LIB_NAME

target.path = ../../../wheatis_demo/clients/
INSTALLS += target
