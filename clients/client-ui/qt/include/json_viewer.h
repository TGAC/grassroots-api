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
#ifndef JSON_VIEWER_H
#define JSON_VIEWER_H

#include <QTreeWidget>
#include <QTextEdit>

#include "jansson.h"

class JSONViewer : public QWidget
{
Q_OBJECT

public:
	JSONViewer (QWidget *parent_p = 0);

	void SetJSONData (const json_t *data_p);

private:
	QTreeWidget *jv_tree_p;
	QTextEdit *jv_viewer_p;
	const json_t *jv_data_p;
};

#endif // JSON_VIEWER_H
