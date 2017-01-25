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
#ifndef MATCHED_SERVICE_LIST_WIDGET_ITEM_H
#define MATCHED_SERVICE_LIST_WIDGET_ITEM_H

#include <QObject>

#include "json_list_widget_item.h"
#include "results_list.h"

#include "results_widget.h"


class ServiceListWidgetItem : public JSONListWidgetItem
{
	Q_OBJECT

public:
	ServiceListWidgetItem (const QString &text_r, QListWidget *parent_p, int type = UserType);
	virtual ~ServiceListWidgetItem ();

	virtual void ShowData ();

	void SetResultsWidget (ResultsWidget *widget_p);

signals:
	void ServiceRequested (const char *service_name_s, const json_t *params_json_p);

};

#endif // MATCHED_SERVICE_LIST_WIDGET_ITEM_H

