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
#include "matched_service_list_widget_item.h"


ServiceListWidgetItem :: ServiceListWidgetItem (const QString &text_r, QListWidget *parent_p, int type)
: JSONListWidgetItem (text_r, parent_p, type)
{
}


ServiceListWidgetItem :: ~ServiceListWidgetItem ()
{
}


void ServiceListWidgetItem :: ShowData ()
{
	/*
	 * Load the parameters into the appropraite Service
	 * in the prefs window and display it
	 */
	QString s = text ();
	QByteArray ba = s.toLocal8Bit ();
	const char *service_name_s = ba.constData ();

	emit ServiceRequested (service_name_s, jlwi_json_data_p);
}
