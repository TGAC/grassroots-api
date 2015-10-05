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


JSONListWidgetItem :: JSONListWidgetItem (const QString &text_r, QListWidget *parent_p, int type)
: QListWidgetItem (text_r, parent_p, type),
	jlwi_json_data_p (0)
{

}


void JSONListWidgetItem :: SetJSONData (const json_t *data_p)
{
	jlwi_json_data_p = data_p;
}



const json_t *JSONListWidgetItem :: GetJSONData () const
{
	return jlwi_json_data_p;
}

