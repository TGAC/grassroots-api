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
#ifndef STANDARD_LIST_WIDGET_ITEM_H
#define STANDARD_LIST_WIDGET_ITEM_H


#include <QListWidgetItem>
#include <QObject>

#include "results_list.h"


class StandardListWidgetItem : public QObject, public QListWidgetItem
{
	Q_OBJECT

signals:
	void WebLinkSelected (const char * const uri_s);

public:
	StandardListWidgetItem (const QString &text_r, QListWidget *parent_p = 0, int type = UserType);
	virtual ~StandardListWidgetItem ();

	virtual void ShowData ();

};

#endif // STANDARD_LIST_WIDGET_ITEM_H

