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
#ifndef RESULTS_WIDGET_H
#define RESULTS_WIDGET_H

#include <QTabWidget>
#include <QString>

#include "results_page.h"

#include "jansson.h"
#include "typedefs.h"

class ResultsWidget : public QTabWidget
{
	Q_OBJECT

public:
  ResultsWidget (QWidget *parent_p = 0);
  ~ResultsWidget ();

  uint32 AddAllResultsPagesFromJSON (const json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s);
  bool AddResultsPageFromJSON (const json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s);
  void ClearData ();

  bool AddInterestedService (json_t *job_p, const char *service_name_s);

	bool AddItemToResultsList (const json_t *results_json_p);

signals:
	void ServiceRequested (const char *service_name_s, const json_t *params_json_p);
	void RunServiceRequested (json_t *service_json_p);


public slots:
  void SelectService (const char *service_name_s, const json_t *params_json_p);
	void RunService (json_t *service_json_p);

private:
	ResultsPage *CreatePageFromJSON (const json_t *json_p, const char * const job_name_s, const char * const service_name_s, const char * const description_s, const char * const uri_s);

	static const char *RW_SERVICES_TAB_TITLE_S;

};

#endif // RESULTS_WIDGET_H
