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

#include "param_line_edit.h"
#include "prefs_widget.h"

#include "string_utils.h"


ParamLineEdit :: ParamLineEdit (Parameter * const param_p, const PrefsWidget * const options_widget_p, QLineEdit :: EchoMode echo, QWidget *parent_p)
:		BaseParamWidget (param_p, options_widget_p)
{
	ple_text_box_p = new QLineEdit (parent_p);
	ple_text_box_p -> setEchoMode (echo);

}


ParamLineEdit ::	~ParamLineEdit ()
{
	delete ple_text_box_p;
	ple_text_box_p = NULL;
}

void ParamLineEdit :: RemoveConnection ()
{
}


void ParamLineEdit :: SetDefaultValue ()
{

	if (bpw_param_p -> pa_type == PT_CHAR)
		{
			char value_s [2];

			*value_s = bpw_param_p -> pa_default.st_char_value;
			* (value_s + 1) = '\0';

			ple_text_box_p -> setText (value_s);

		}
	else
		{
			const char *value_s = bpw_param_p -> pa_default.st_string_value_s;

			ple_text_box_p -> setText (value_s);
		}
}


QWidget *ParamLineEdit :: GetQWidget ()
{
	return ple_text_box_p;
}

void ParamLineEdit :: SetMaxLength (int l)
{
	ple_text_box_p -> setMaxLength (l);
}


bool ParamLineEdit :: SetValueFromText (const char *value_s)
{
	QString s = ple_text_box_p -> text ();
	qDebug () << "old " << s;

	ple_text_box_p -> setText (value_s);

	s = ple_text_box_p -> text ();
	qDebug () << "new " << s;

	return true;
}



bool ParamLineEdit :: SetValueFromJSON (const json_t * const value_p)
{
	bool success_flag = false;

	if (json_is_string (value_p))
		{
			const char *value_s = json_string_value (value_p);

			ple_text_box_p -> setText (value_s);
			success_flag = true;
		}

	return success_flag;
}



bool ParamLineEdit :: StoreParameterValue ()
{
	QString s = ple_text_box_p -> text ();
	QByteArray ba = s.toLocal8Bit ();
	const char *value_s = ba.constData ();

	return UpdateConfigValue (value_s);
}


bool ParamLineEdit :: UpdateConfigValue (const char * const value_s)
{
	bool success_flag = SetParameterValue (bpw_param_p, value_s, true);

	qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value_s;

	return success_flag;
}



