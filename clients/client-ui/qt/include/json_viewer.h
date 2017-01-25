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
#ifndef JSON_VIEWER_H
#define JSON_VIEWER_H

#include <QTreeWidget>
#include <QTextEdit>
#include <QMenu>


#include "jansson.h"
#include "viewable_widget.h"


class JSONViewer : public QWidget, public ViewableWidget
{
Q_OBJECT

public:
	JSONViewer (QWidget *parent_p = 0);

	void SetJSONData (json_t *data_p);

	virtual char *GetText () const;

	virtual QWidget *GetWidget ();

	void MakeGrassrootsServicesRunnable ();


signals:
	void RunServiceRequested (json_t *request_p);


private:
	QTreeWidget *jv_tree_p;
	QTextEdit *jv_viewer_p;
	const json_t *jv_data_p;

	QTreeWidgetItem *InsertData (QTreeWidgetItem *parent_p, const char *key_s, json_t *data_p);
	void AddTopLevelNode (const char *key_s, json_t *data_p);

	void AddActions (QMenu *menu_p);

private slots:
	void PrepareMenu (const QPoint &pos_r);
	void RunLinkedService (bool checked_flag);
	void ResizeColumns (const QModelIndex &index);

	void SetSystemFont ();
	void SetFixedFont ();
};

#endif // JSON_VIEWER_H
