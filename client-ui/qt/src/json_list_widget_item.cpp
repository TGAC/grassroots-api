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
#include "json_list_widget_item.h"
#include "text_viewer.h"
#include "viewer_widget.h"


JSONListWidgetItem :: JSONListWidgetItem (const QString &text_r, QListWidget *parent_p, ResultsList *grandparent_p, int type)
: StandardListWidgetItem (text_r, parent_p, grandparent_p, type),
	jlwi_json_data_p (0)
{

}


JSONListWidgetItem :: ~ JSONListWidgetItem ()
{
	if (jlwi_json_data_p)
		{
			json_decref (jlwi_json_data_p);
		}
}


bool JSONListWidgetItem :: SetJSONData (const json_t *data_p)
{
	jlwi_json_data_p = json_deep_copy (data_p);
}



const json_t *JSONListWidgetItem :: GetJSONData () const
{
	return jlwi_json_data_p;
}



void JSONListWidgetItem :: ShowData ()
{
	/* Open the json object */
	TextViewer *text_viewer_p = new TextViewer;
	ViewerWidget *viewer_widget_p = new ViewerWidget (text_viewer_p, listWidget ());

	if (json_is_string (jlwi_json_data_p))
		{
			const char *data_s = json_string_value (jlwi_json_data_p);
			text_viewer_p -> SetText (data_s);
		}
	else
		{
			char *data_s = json_dumps (jlwi_json_data_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

			if (data_s)
				{
					text_viewer_p -> SetText (data_s);
					free (data_s);
				}
		}

	viewer_widget_p -> show ();
}
