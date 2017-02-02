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
#include "json_list_widget_item.h"
#include "text_viewer.h"
#include "viewer_widget.h"
#include "json_util.h"
#include "json_viewer.h"


JSONListWidgetItem :: JSONListWidgetItem (const QString &text_r, QListWidget *parent_p, int type)
: StandardListWidgetItem (text_r, parent_p, type),
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
	bool success_flag = false;
	json_t *new_data_p = json_deep_copy (data_p);

	if (new_data_p)
		{
			if (jlwi_json_data_p)
				{
					json_decref (jlwi_json_data_p);
				}

			jlwi_json_data_p = new_data_p;

			success_flag = true;
		}

	return success_flag;
}



const json_t *JSONListWidgetItem :: GetJSONData () const
{
	return jlwi_json_data_p;
}


void JSONListWidgetItem :: RunLinkedService (json_t *request_p)
{
	emit RunServiceRequested (request_p);
}


void JSONListWidgetItem :: ShowData ()
{
	/* Open the json object */
	ViewableWidget *viewer_p = 0;

	if (json_is_string (jlwi_json_data_p))
		{
			const char *data_s = json_string_value (jlwi_json_data_p);
			TextViewer *text_viewer_p = new TextViewer;

			text_viewer_p -> SetText (data_s);
			viewer_p = text_viewer_p;
		}
	else if ((json_is_object (jlwi_json_data_p)) || (json_is_array (jlwi_json_data_p)))
		{
			JSONViewer *json_viewer_p = new JSONViewer;

			json_viewer_p -> SetJSONData (jlwi_json_data_p);

			connect (json_viewer_p, &JSONViewer :: RunServiceRequested, this, &JSONListWidgetItem :: RunLinkedService);

			viewer_p = json_viewer_p;
		}


	if (viewer_p)
		{
			ViewerWidget *viewer_widget_p = new ViewerWidget (viewer_p, listWidget ());

			viewer_widget_p -> setWindowTitle (text ());
			viewer_widget_p -> show ();

		}
}


bool JSONListWidgetItem :: AddLinkedService (const json_t *linked_service_p)
{
	bool success_flag = false;


	//results_p -> AddInterestedService (job_p, service_name_s);

	return success_flag;
}



void JSONListWidgetItem :: ParseLinkedServices (const json_t *data_p)
{
	const json_t *linked_services_p = json_object_get (data_p, LINKED_SERVICES_S);

	if (linked_services_p)
		{
			if (json_is_array (linked_services_p))
				{
					size_t i;
					json_t *linked_service_p;

					json_array_foreach (linked_services_p, i, linked_service_p)
						{
							if (AddLinkedService (linked_service_p))
								{

								}
						}
				}
			else
				{
					if (AddLinkedService (linked_services_p))
						{

						}
				}

		}
}
