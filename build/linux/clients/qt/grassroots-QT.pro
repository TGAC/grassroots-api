#-------------------------------------------------
#
# Project created by QtCreator 2014-09-11T12:08:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webenginewidgets

TARGET = grassroots-qt-client
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


include(dependencies.pri)


DIR_GRASSROOTS_SHARED_SRC = $$DIR_GRASSROOTS_PKG_ROOT/shared/src
DIR_GRASSROOTS_SERVER_SRC = $$DIR_GRASSROOTS_PKG_ROOT/server/src

DIR_GRASSROOTS_EXTRAS = $$DIR_GRASSROOTS_INSTALL_ROOT/extras

DIR_GRASSROOTS_PARAMS = $$DIR_GRASSROOTS_SHARED_SRC/parameters
DIR_GRASSROOTS_PARAMS_INC = $$DIR_GRASSROOTS_PARAMS/include
GRASSROOTS_PARAMS_LIB_NAME = grassroots_params

DIR_GRASSROOTS_CLIENTS = $$DIR_GRASSROOTS_PKG_ROOT/clients/lib
DIR_GRASSROOTS_CLIENTS_INC = $$DIR_GRASSROOTS_CLIENTS/include
GRASSROOTS_CLIENTS_LIB_NAME = grassroots_clients

DIR_GRASSROOTS_UTIL = $$DIR_GRASSROOTS_SHARED_SRC/util
DIR_GRASSROOTS_UTIL_INC = $$DIR_GRASSROOTS_UTIL/include
GRASSROOTS_UTIL_LIB_NAME = grassroots_util

DIR_GRASSROOTS_SERVER = $$DIR_GRASSROOTS_SERVER_SRC/lib
DIR_GRASSROOTS_SERVER_INC = $$DIR_GRASSROOTS_SERVER/include
GRASSROOTS_SERVER_LIB_NAME = grassroots_server

DIR_GRASSROOTS_NETWORK = $$DIR_GRASSROOTS_SHARED_SRC/network
DIR_GRASSROOTS_NETWORK_INC = $$DIR_GRASSROOTS_NETWORK/include
GRASSROOTS_NETWORK_LIB_NAME = grassroots_network

DIR_GRASSROOTS_SERVICE = $$DIR_GRASSROOTS_SERVER_SRC/services/lib
DIR_GRASSROOTS_SERVICE_INC = $$DIR_GRASSROOTS_SERVICE/include
GRASSROOTS_SERVICE_LIB_NAME = grassroots_service

DIR_GRASSROOTS_HANDLER = $$DIR_GRASSROOTS_SHARED_SRC/handlers/lib
DIR_GRASSROOTS_HANDLER_INC = $$DIR_GRASSROOTS_HANDLER/include
GRASSROOTS_HANDLER_LIB_NAME = grassroots_handler

DIR_GRASSROOTS_UUID = $$DIR_GRASSROOTS_EXTRAS/libuuid
DIR_GRASSROOTS_UUID_INC = $$DIR_GRASSROOTS_UUID/include
DIR_GRASSROOTS_UUID_LIB = $$DIR_GRASSROOTS_UUID/lib
GRASSROOTS_UUID_LIB_NAME = uuid


DIR_GRASSROOTS_JANSSON = $$DIR_GRASSROOTS_EXTRAS/jansson
DIR_GRASSROOTS_JANSSON_INC = $$DIR_GRASSROOTS_JANSSON/include
DIR_GRASSROOTS_JANSSON_LIB = $$DIR_GRASSROOTS_JANSSON/lib
GRASSROOTS_JANSSON_LIB_NAME = jansson

DEFINES += "UNIX" "_DEBUG"

INCLUDEPATH += \
	$$DIR_GRASSROOTS_QT_SRC/include \
  $$DIR_GRASSROOTS_UUID_INC \
  $$DIR_GRASSROOTS_JANSSON_INC \
  $$DIR_GRASSROOTS_CLIENTS_INC \
  $$DIR_GRASSROOTS_PARAMS_INC \
  $$DIR_GRASSROOTS_UTIL_INC \
  $$DIR_GRASSROOTS_UTIL_INC/containers \
  $$DIR_GRASSROOTS_UTIL_INC/io \
  $$DIR_GRASSROOTS_SERVICE_INC \
  $$DIR_GRASSROOTS_NETWORK_INC \
  $$DIR_GRASSROOTS_HANDLER_INC \



SOURCES += \
		$$DIR_GRASSROOTS_QT_SRC/src/base_param_widget.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/file_chooser_widget.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/param_check_box.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/param_combo_box.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/param_double_spin_box.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/param_spin_box.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/param_line_edit.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/param_text_box.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/prefs_widget.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/qt_parameter_widget.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/client_ui_api.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/service_prefs_widget.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/results_list.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/results_widget.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/json_list_widget_item.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/json_viewer.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/text_viewer.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/viewer_widget.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/viewable_widget.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/main_window.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/matched_service_list_widget_item.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/services_list.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/services_tabs.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/results_window.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/keyword_widget.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/ui_utils.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/param_group_box.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/progress_window.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/progress_widget.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/param_table_widget.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/param_json_editor.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/qt_client_data.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/servce_ui.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/standard_list_widget_item.cpp \
		$$DIR_GRASSROOTS_QT_SRC/src/results_page.cpp

HEADERS  += \
		$$DIR_GRASSROOTS_QT_SRC/include/base_param_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/file_chooser_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/param_check_box.h \
		$$DIR_GRASSROOTS_QT_SRC/include/param_combo_box.h \
		$$DIR_GRASSROOTS_QT_SRC/include/param_double_spin_box.h \
		$$DIR_GRASSROOTS_QT_SRC/include/param_spin_box.h \
		$$DIR_GRASSROOTS_QT_SRC/include/param_text_box.h \
		$$DIR_GRASSROOTS_QT_SRC/include/param_line_edit.h \
		$$DIR_GRASSROOTS_QT_SRC/include/prefs_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/qt_parameter_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/client_ui_api.h \
		$$DIR_GRASSROOTS_QT_SRC/include/service_prefs_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/results_list.h \
		$$DIR_GRASSROOTS_QT_SRC/include/results_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/json_list_widget_item.h \
		$$DIR_GRASSROOTS_QT_SRC/include/json_viewer.h \
		$$DIR_GRASSROOTS_QT_SRC/include/text_viewer.h \
		$$DIR_GRASSROOTS_QT_SRC/include/viewer_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/viewable_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/main_window.h \
		$$DIR_GRASSROOTS_QT_SRC/include/services_list.h \
		$$DIR_GRASSROOTS_QT_SRC/include/services_tabs.h \
		$$DIR_GRASSROOTS_QT_SRC/include/service_ui.h \
		$$DIR_GRASSROOTS_QT_SRC/include/results_window.h \
		$$DIR_GRASSROOTS_QT_SRC/include/qt_client_data.h \
		$$DIR_GRASSROOTS_QT_SRC/include/keyword_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/runnable_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/ui_utils.h \
		$$DIR_GRASSROOTS_QT_SRC/include/param_group_box.h \
		$$DIR_GRASSROOTS_QT_SRC/include/progress_window.h \
		$$DIR_GRASSROOTS_QT_SRC/include/progress_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/droppable_target_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/param_table_widget.h \
		$$DIR_GRASSROOTS_QT_SRC/include/param_json_editor.h \
		$$DIR_GRASSROOTS_QT_SRC/include/matched_service_list_widget_item.h \
		$$DIR_GRASSROOTS_QT_SRC/include/standard_list_widget_item.h \
		$$DIR_GRASSROOTS_QT_SRC/include/results_page.h


LIBS += -L$$DIR_GRASSROOTS_LIBS \
 -l$$GRASSROOTS_UTIL_LIB_NAME \
 -l$$GRASSROOTS_SERVICE_LIB_NAME \
 -l$$GRASSROOTS_CLIENTS_LIB_NAME \
 -l$$GRASSROOTS_PARAMS_LIB_NAME \
 -L$$DIR_GRASSROOTS_UUID_LIB -l$$GRASSROOTS_UUID_LIB_NAME \
 -L$$DIR_GRASSROOTS_JANSSON_LIB  -l$$GRASSROOTS_JANSSON_LIB_NAME \
 -lpthread

target.path = $$DIR_GRASSROOTS_INSTALL_ROOT/clients/
INSTALLS += target

OTHER_FILES += \
    dependencies.pri \
    example_dependencies.pri
