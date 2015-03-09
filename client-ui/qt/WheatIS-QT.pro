#-------------------------------------------------
#
# Project created by QtCreator 2014-09-11T12:08:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = wheatis-qt-client
TEMPLATE = lib

VPATH += src include

unix {
 CONFIG(release, debug|release) {
	BUILD = release
 }
 CONFIG(debug, debug|release) {
	BUILD = debug
	DEFINES += "_DEBUG"
 }
}


DIR_WHEATIS = ../..


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

DIR_WHEATIS_NETWORK = $$DIR_WHEATIS/network
DIR_WHEATIS_NETWORK_INC = $$DIR_WHEATIS_NETWORK/include
DIR_WHEATIS_NETWORK_LIB = $$DIR_WHEATIS_NETWORK/$$BUILD
WHEATIS_NETWORK_LIB_NAME = wheatis_network

DIR_WHEATIS_SERVICE = $$DIR_WHEATIS/services/lib
DIR_WHEATIS_SERVICE_INC = $$DIR_WHEATIS_SERVICE/include
DIR_WHEATIS_SERVICE_LIB = $$DIR_WHEATIS_SERVICE/$$BUILD
WHEATIS_SERVICE_LIB_NAME = wheatis_service

DIR_WHEATIS_HANDLER = $$DIR_WHEATIS/handlers/lib
DIR_WHEATIS_HANDLER_INC = $$DIR_WHEATIS_HANDLER/include
DIR_WHEATIS_HANDLER_LIB = $$DIR_WHEATIS_HANDLER/$$BUILD
WHEATIS_HANDLER_LIB_NAME = wheatis_handler


DEFINES += "UNIX"

INCLUDEPATH += include \
	$$DIR_WHEATIS_UTIL_INC \
	$$DIR_WHEATIS_UTIL_INC/containers \
	$$DIR_WHEATIS_UTIL_INC/io \
	$$DIR_WHEATIS_SERVICE_INC \
	$$DIR_WHEATIS_NETWORK_INC \
	$$DIR_WHEATIS_HANDLER_INC


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
    src/client_ui_api.cpp \
    src/service_prefs_widget.cpp \
    src/results_list.cpp \
    src/results_widget.cpp \
    src/json_list_widget_item.cpp \
    src/json_viewer.cpp \
    src/text_viewer.cpp \
    src/viewer_widget.cpp \
    src/viewable_widget.cpp \
    src/main_window.cpp \
    src/services_list.cpp \
    src/services_tabs.cpp

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
    include/client_ui_api.h \
    include/service_prefs_widget.h \
    include/results_list.h \
    include/results_widget.h \
    include/json_list_widget_item.h \
    include/json_viewer.h \
    include/text_viewer.h \
    include/viewer_widget.h \
    include/viewable_widget.h \
    include/main_window.h \
    include/services_list.h \
    include/services_tabs.h \
    include/service_ui.h

LIBS += -L$$DIR_WHEATIS_UTIL_LIB  -l$$WHEATIS_UTIL_LIB_NAME \
 -L$$DIR_WHEATIS_SERVICE_LIB  -l$$WHEATIS_SERVICE_LIB_NAME \
   -lpthread -ljansson

target.path = ../../../wheatis_demo/clients/
INSTALLS += target
