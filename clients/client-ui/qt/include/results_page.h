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
#ifndef RESULTS_PAGE_H
#define RESULTS_PAGE_H

#include <QLabel>
#include <QList>
#include <QWebEngineView>
#include <QWidget>

#include "results_list.h"

class ResultsWidget;

class ResultsPage: public QWidget
{
	Q_OBJECT

public:
	ResultsPage (ResultsWidget *parent_p = 0);
	ResultsPage (const json_t *results_list_json_p, const char *job_name_s, const char * const service_name_s, const char * const description_s, const char * const uri_s, ResultsWidget *parent_p = 0);
  ~ResultsPage ();

  ResultsList *GetResultsList () const;

signals:
	void ServiceRequested (const char *service_name_s, const json_t *params_json_p);
	void RunServiceRequested (json_t *request_p);

public slots:
	void OpenWebLink (const char * const uri_s);
	void SelectService (const char *service_name_s, const json_t *params_json_p);
	void RunService (json_t *request_p);

private:
  QList <QWebEngineView *> rp_browsers;
  ResultsList *rp_results_list_p;
	const char *rp_job_name_s;

	void SetUp (ResultsWidget *parent_p, const char * const job_name_s, const char *service_name_s, const char * const description_s, const char * const uri_s);
};

#endif // RESULTS_PAGE_H
