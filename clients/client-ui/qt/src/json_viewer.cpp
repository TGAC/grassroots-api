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
#include <QHBoxLayout>

#include "json_viewer.h"




JSONViewer ::	JSONViewer (QWidget *parent_p)
: QWidget (parent_p)
{
	QHBoxLayout *layout_p = new QHBoxLayout;

	jv_tree_p = new QTreeWidget (this);
	layout_p -> addWidget (jv_tree_p);

	jv_viewer_p = new QTextEdit (this);
	jv_viewer_p -> setReadOnly (true);
	layout_p -> addWidget (jv_viewer_p);

	setLayout (layout_p);

	setWindowTitle ("JSON Viewer");

}



void JSONViewer :: SetJSONData (const json_t *data_p)
{
	size_t size = 1;

	jv_data_p = data_p;

	jv_tree_p -> clear ();
	jv_viewer_p -> clear ();

	if (json_is_array (data_p))
		{
			size = json_array_size (data_p);
			jv_tree_p -> setColumnCount (size);
		}
	else
		{
			jv_tree_p -> setColumnCount (1);
		}

	char *value_s = json_dumps (data_p, 0);
	if (value_s)
		{
			jv_viewer_p -> setPlainText (value_s);
			free (value_s);
		}

}