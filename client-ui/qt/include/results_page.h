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
#ifndef RESULTS_PAGE_H
#define RESULTS_PAGE_H

#include <QLabel>
#include <QWidget>

#include "results_list.h"


class ResultsPage: public QWidget
{
public:
  ResultsPage (QWidget *parent_p = 0);
  ResultsPage (const json_t *results_list_json_p, const char * const description_s, const char * const uri_s, QWidget *parent_p = 0);
  ~ResultsPage ();

  ResultsList *GetResultsList () const;

private:
  ResultsList *rp_results_list_p;
  QLabel *rp_description_label_p;
  QLabel *rp_uri_label_p;
};

#endif // RESULTS_PAGE_H
