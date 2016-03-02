#-------------------------------------------------
#
# Project created by QtCreator 2014-09-11T12:08:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

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


DIR_GRASSROOTS_EXTRAS = $$DIR_GRASSROOTS_INSTALL_ROOT/extras

DIR_GRASSROOTS_PARAMS = $$DIR_GRASSROOTS/parameters
DIR_GRASSROOTS_PARAMS_INC = $$DIR_GRASSROOTS_PARAMS/include
DIR_GRASSROOTS_PARAMS_LIB = $$DIR_GRASSROOTS_PARAMS/$$BUILD
GRASSROOTS_PARAMS_LIB_NAME = grassroots_params

DIR_GRASSROOTS_CLIENTS = $$DIR_GRASSROOTS/clients/lib
DIR_GRASSROOTS_CLIENTS_INC = $$DIR_GRASSROOTS_CLIENTS/include
DIR_GRASSROOTS_CLIENTS_LIB = $$DIR_GRASSROOTS_CLIENTS/$$BUILD
GRASSROOTS_CLIENTS_LIB_NAME = grassroots_clients


DIR_GRASSROOTS_UTIL = $$DIR_GRASSROOTS/util
DIR_GRASSROOTS_UTIL_INC = $$DIR_GRASSROOTS_UTIL/include
DIR_GRASSROOTS_UTIL_LIB = $$DIR_GRASSROOTS_UTIL/$$BUILD
GRASSROOTS_UTIL_LIB_NAME = grassroots_util

DIR_GRASSROOTS_SERVER = $$DIR_GRASSROOTS/server
DIR_GRASSROOTS_SERVER_INC = $$DIR_GRASSROOTS_SERVER/include
DIR_GRASSROOTS_SERVER_LIB = $$DIR_GRASSROOTS_SERVER/$$BUILD
GRASSROOTS_SERVER_LIB_NAME = grassroots_server

DIR_GRASSROOTS_NETWORK = $$DIR_GRASSROOTS/network
DIR_GRASSROOTS_NETWORK_INC = $$DIR_GRASSROOTS_NETWORK/include
DIR_GRASSROOTS_NETWORK_LIB = $$DIR_GRASSROOTS_NETWORK/$$BUILD
GRASSROOTS_NETWORK_LIB_NAME = grassroots_network

DIR_GRASSROOTS_SERVICE = $$DIR_GRASSROOTS/services/lib
DIR_GRASSROOTS_SERVICE_INC = $$DIR_GRASSROOTS_SERVICE/include
DIR_GRASSROOTS_SERVICE_LIB = $$DIR_GRASSROOTS_SERVICE/$$BUILD
GRASSROOTS_SERVICE_LIB_NAME = grassroots_service

DIR_GRASSROOTS_HANDLER = $$DIR_GRASSROOTS/handlers/lib
DIR_GRASSROOTS_HANDLER_INC = $$DIR_GRASSROOTS_HANDLER/include
DIR_GRASSROOTS_HANDLER_LIB = $$DIR_GRASSROOTS_HANDLER/$$BUILD
GRASSROOTS_HANDLER_LIB_NAME = grassroots_handler

DIR_GRASSROOTS_UUID = $$DIR_GRASSROOTS_EXTRAS/libuuid
DIR_GRASSROOTS_UUID_INC = $$DIR_GRASSROOTS_UUID/include
DIR_GRASSROOTS_UUID_LIB = $$DIR_GRASSROOTS_UUID/lib
GRASSROOTS_UUID_LIB_NAME = uuid


DEFINES += "UNIX" "_DEBUG"

INCLUDEPATH += include \
  $$DIR_GRASSROOTS_UUID_INC \
  $$DIR_GRASSROOTS_CLIENTS_INC \
  $$DIR_GRASSROOTS_PARAMS_INC \
  $$DIR_GRASSROOTS_UTIL_INC \
  $$DIR_GRASSROOTS_UTIL_INC/containers \
  $$DIR_GRASSROOTS_UTIL_INC/io \
  $$DIR_GRASSROOTS_SERVICE_INC \
  $$DIR_GRASSROOTS_NETWORK_INC \
  $$DIR_GRASSROOTS_HANDLER_INC \



SOURCES += \
    src/base_param_widget.cpp \
    src/file_chooser_widget.cpp \
    src/param_check_box.cpp \
    src/param_combo_box.cpp \
    src/param_double_spin_box.cpp \
    src/param_spin_box.cpp \
    src/param_line_edit.cpp \
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
    src/services_tabs.cpp \
    src/results_window.cpp \
    src/keyword_widget.cpp \
    src/ui_utils.cpp \
    src/param_group_box.cpp \
    src/progress_window.cpp \
    src/progress_widget.cpp \
    src/param_table_widget.cpp \
    src/param_json_editor.cpp \
    src/matched_service_list_widget_item.cpp \
    src/servce_ui.cpp \
    src/standard_list_widget_item.cpp

HEADERS  += \
    include/base_param_widget.h \
    include/file_chooser_widget.h \
    include/param_check_box.h \
    include/param_combo_box.h \
    include/param_double_spin_box.h \
    include/param_spin_box.h \
    include/param_text_box.h \
    include/param_line_edit.h \
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
    include/service_ui.h \
    include/results_window.h \
    include/qt_client_data.h \
    include/keyword_widget.h \
    include/runnable_widget.h \
    include/ui_utils.h \
    include/param_group_box.h \
    include/progress_window.h \
    include/progress_widget.h \
    include/droppable_target_widget.h \
    include/param_table_widget.h \
    include/param_json_editor.h \
    include/matched_service_list_widget_item.h \
		include/standard_list_widget_item.h


LIBS += -L$$DIR_GRASSROOTS_UTIL_LIB  -l$$GRASSROOTS_UTIL_LIB_NAME \
 -L$$DIR_GRASSROOTS_SERVICE_LIB  -l$$GRASSROOTS_SERVICE_LIB_NAME \
 -L$$DIR_GRASSROOTS_CLIENTS_LIB  -l$$GRASSROOTS_CLIENTS_LIB_NAME \
 -L$$DIR_GRASSROOTS_PARAMS_LIB  -l$$GRASSROOTS_PARAMS_LIB_NAME \
 -L$$DIR_GRASSROOTS_UUID_LIB  -l$$GRASSROOTS_UUID_LIB_NAME \
	 -lpthread -ljansson

target.path = $$DIR_GRASSROOTS_INSTALL_ROOT/clients/
INSTALLS += target

OTHER_FILES += \
    dependencies.pri \
    example_dependencies.pri
