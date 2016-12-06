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
#include <QDebug>
#include <QDialog>

#include "param_json_editor.h"

#include "jansson.h"

#include "json_util.h"
#include "string_utils.h"


DroppableJSONBox :: DroppableJSONBox (QWidget *parent_p)
	: DroppableTextBox (parent_p)
{}


bool DroppableJSONBox :: SetFromText (const char * const data_s)
{
	bool success_flag = false;
	json_error_t error;
	json_t *data_p = json_loads (data_s, 0, &error);

	if (data_p)
		{
			clear ();
			insertPlainText (data_s);
			success_flag = true;

			json_decref (data_p);
		}

	return success_flag;
}


bool DroppableJSONBox :: SetFromJSON (const json_t * const value_p)
{
	bool success_flag = false;
	char *data_s = json_dumps (value_p, 0);

	if (data_s)
		{
			clear ();
			insertPlainText (data_s);
			success_flag = true;

			free (data_s);
		}

	return success_flag;
}


ParamJSONEditor :: ParamJSONEditor (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
:	ParamTextBox (param_p, options_widget_p, parent_p)
{
}



bool ParamJSONEditor :: CreateDroppableTextBox (QWidget *parent_p)
{
	ptb_text_box_p =  new DroppableJSONBox (parent_p);

	return true;
}



ParamJSONEditor :: ~ParamJSONEditor ()
{

}



void ParamJSONEditor :: SetDefaultValue ()
{
	if (!IsJSONEmpty (bpw_param_p -> pa_default.st_json_p))
		{
			char *value_s = json_dumps (bpw_param_p -> pa_default.st_json_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

			if (value_s)
				{
					ptb_text_box_p -> setPlainText (value_s);
					free (value_s);
				}
		}
}



bool ParamJSONEditor :: StoreParameterValue ()
{
	bool success_flag = false;
	QString s = ptb_text_box_p -> toPlainText ();
	QByteArray ba = s.toLocal8Bit ();
	const char *value_s = ba.constData ();

	if (!IsStringEmpty (value_s))
		{
			json_error_t error;
			json_t *data_p = json_loads (value_s, 0, &error);

			if (data_p)
				{
					success_flag = SetParameterValue (bpw_param_p, data_p, true);

					if (!success_flag)
						{
							json_decref (data_p);
						}
				}
			else
				{
					SetParameterValue (bpw_param_p, NULL, true);
				}
		}
	else
		{
			success_flag = SetParameterValue (bpw_param_p, NULL, true);
		}

	qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value_s << " " << success_flag;

	if (!success_flag)
		{

		}

	return success_flag;
}



bool ParamJSONEditor :: SetValueFromText (const char *value_s)
{
	return ptb_text_box_p -> SetFromText (value_s);
}



bool ParamJSONEditor :: SetValueFromJSON (const json_t * const value_p)
{
	return (static_cast <DroppableJSONBox *> (ptb_text_box_p)) -> SetFromJSON (value_p);
}
