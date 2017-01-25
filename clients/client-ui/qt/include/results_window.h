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
#ifndef RESULTS_WINDOW_H
#define RESULTS_WINDOW_H

#include <QMainWindow>
#include <QWindow>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "results_widget.h"
#include "qt_client_data.h"

#include "typedefs.h"
#include "jansson.h"


class MainWindow;

class ResultsWindow : public QWidget
{
public:
	ResultsWindow (MainWindow *parent_p, QTClientData *client_data_p);
  ~ResultsWindow ();
	uint32 AddAllResultsPagesFromJSON (json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s);
	bool AddResultsPageFromJSON (json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s);
	void ClearData ();

	bool AddInterestedService (json_t *job_p, const char *service_name_s);


protected:
	virtual void dropEvent (QDropEvent *event_p);
	virtual void dragEnterEvent (QDragEnterEvent *event_p);

private:

	bool AddData (json_t *data_p);

	void LoadResults (const char * const filename_s);
	void LoadResults ();
	bool AddJSONDataToArray (json_t *results_p, const json_t *doc_p);
	void SaveJSONResults (const json_t * const results_p);



private slots:
  void SaveAllResults (bool clicked_flag);
  void SaveResults (bool clicked_flag);
  void SaveCompactResults (bool clicked_flag);

private:
  ResultsWidget *rw_results_p;
	json_t *rw_data_p;
	QTClientData *rw_client_data_p;
};


#endif // RESULTS_WINDOW_H

