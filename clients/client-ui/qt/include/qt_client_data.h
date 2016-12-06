/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/**
 * @file
 * @brief
 */
#ifndef QT_CLIENT_DATA_H
#define QT_CLIENT_DATA_H

#include <QApplication>
#include <QLinkedList>

#include "client.h"

#include "main_window.h"
#include "results_window.h"
#include "progress_window.h"
#include "viewer_widget.h"

// forward declaration
class ResultsWindow;


typedef struct QTClientData
{
	ClientData qcd_base_data;
	QApplication *qcd_app_p;
	MainWindow *qcd_window_p;
	ProgressWindow *qcd_progress_p;
	QLinkedList <ViewerWidget *> *qcd_viewer_widgets_p;
	QLinkedList <ResultsWindow *> *qcd_results_widgets_p;
	char *qcd_dummy_arg_s;
	bool qcd_init_flag;
} QTClientData;





ResultsWindow *GetNewResultsWindow (QTClientData *client_data_p);


void RemoveResultsWindowFromQTClientData (QTClientData *client_data_p, ResultsWindow *results_p);


#endif // QT_CLIENT_DATA_H

