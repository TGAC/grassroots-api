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

#include <QVariant>

#include "standard_list_widget_item.h"
#include "data_resource.h"


StandardListWidgetItem :: StandardListWidgetItem (const QString &text_r, QListWidget *parent_p, int type)
: QListWidgetItem (text_r, parent_p, type)
{

}


StandardListWidgetItem :: ~StandardListWidgetItem ()
{

}


void StandardListWidgetItem :: ShowData ()
{
	QVariant v = data (Qt :: UserRole);
	QString s = v.toString ();
	QByteArray ba = s.toLocal8Bit ();
	const char *value_s = ba.constData ();

	if ((s.startsWith (PROTOCOL_IRODS_S))  || (s.startsWith (PROTOCOL_FILE_S)))
		{

		}
	else if ((s.startsWith (PROTOCOL_HTTP_S))  || (s.startsWith (PROTOCOL_HTTPS_S)))
		{
			emit WebLinkSelected (value_s);
		}
}
