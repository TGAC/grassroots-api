/*
** Copyright 2014-2015 The Genome Analysis Centre
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
#ifndef RESULTS_WIDGET_H
#define RESULTS_WIDGET_H

#include <QTabWidget>
#include <QString>

#include "jansson.h"
#include "typedefs.h"

class ResultsWidget : public QTabWidget
{
public:
  ResultsWidget (QWidget *parent_p = 0);
  ~ResultsWidget ();

  uint32 AddAllResultsPagesFromJSON (const json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s);
  bool AddResultsPageFromJSON (const json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s);
  void ClearData ();

  bool AddInterestedService (json_t *job_p, const char *service_name_s);


private:
  QWidget *CreatePageFromJSON (const json_t *json_p, const char * const description_s, const char * const uri_s);

  static QString RW_SERVICES_TAB_TITLE;

};

#endif // RESULTS_WIDGET_H
