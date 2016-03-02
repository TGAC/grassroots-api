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
#ifndef RESULTS_LIST_HPP
#define RESULTS_LIST_HPP

#include <QWidget>
#include <QListWidget>
#include <QList>
#include <QWebView>

#include "jansson.h"
#include "results_widget.h"

class ResultsList : public QWidget
{
	Q_OBJECT

public slots:
	bool SetListFromJSON (const json_t *results_list_json_p);
	void SelectService (const char *service_name_s, const json_t *params_json_p);

private slots:
	void OpenItemLink (QListWidgetItem *item_p);

public:

	/*********************/
	/***** FUNCTIONS *****/
	/*********************/

	ResultsList (QWidget *parent_p, ResultsWidget *results_widget_p = 0);

	~ResultsList ();

	bool AddItemFromJSON (const json_t *json_p);

	void ShowWebLink (const QString &link_r);

private:

	/*********************/
	/***** VARIABLES *****/
	/*********************/
	QListWidget *rl_list_p;


	QList <QWebView *> rl_browsers;
	ResultsWidget *rl_grandparent_p;
};


//Q_DECLARE_METATYPE(QList)

#endif		/* #ifndef RESULTS_LIST_HPP */
