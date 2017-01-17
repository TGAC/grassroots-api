/*
** Copyright 2014-2016 The Genome Analysis Centre
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
#ifndef JSON_LIST_WIDGET_ITEM_H
#define JSON_LIST_WIDGET_ITEM_H

#include <QObject>

#include "jansson.h"
#include "results_list.h"
#include "standard_list_widget_item.h"


class JSONListWidgetItem : public StandardListWidgetItem
{
	Q_OBJECT

public:
	JSONListWidgetItem (const QString &text_r, QListWidget *parent_p = 0, int type = UserType);
	virtual ~JSONListWidgetItem ();

	bool SetJSONData (const json_t *data_p);
	const json_t *GetJSONData () const;

	virtual void ShowData ();

protected:
	json_t *jlwi_json_data_p;

	void ParseLinkedServices (const json_t *data_p);

	bool AddLinkedService (const json_t *linked_service_p);

signals:
	void RunServiceRequested (json_t *request_p);

private slots:
	void RunLinkedService (json_t *request_p);

};

#endif // JSON_LIST_WIDGET_ITEM_H
