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
#ifndef RESULTS_LIST_HPP
#define RESULTS_LIST_HPP

#include <QWidget>
#include <QListWidget>
#include <QList>
#include <QWebEngineView>

#include "jansson.h"

class ResultsPage;

class ResultsList : public QWidget
{
	Q_OBJECT

public slots:
	bool SetListFromJSON (const char * const name_s, const json_t *results_list_json_p);

private slots:
	void OpenItemLink (QListWidgetItem *item_p);

public:

	/*********************/
	/***** FUNCTIONS *****/
	/*********************/

	ResultsList (ResultsPage *parent_p);

	~ResultsList ();

	bool AddItemFromJSON (const char * const name_s, const json_t *json_p);


private:

	/*********************/
	/***** VARIABLES *****/
	/*********************/
	ResultsPage *rl_parent_p;
	QListWidget *rl_list_p;

};



#endif		/* #ifndef RESULTS_LIST_HPP */
