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

#include "param_check_box.h"

#include "progress_window.h"
#include "prefs_widget.h"

#include "string_utils.h"


ParamCheckBox :: ParamCheckBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
: BaseParamWidget (param_p, options_widget_p)
{
	pcb_check_box_p = new QCheckBox (parent_p);
}


ParamCheckBox ::	~ParamCheckBox ()
{
	delete pcb_check_box_p;
}



bool ParamCheckBox :: StoreParameterValue ()
{
	bpw_param_p -> pa_current_value.st_boolean_value = pcb_check_box_p -> isChecked ();
	return true;
}


void ParamCheckBox :: SetDefaultValue ()
{
	bool b = bpw_param_p -> pa_default.st_boolean_value;

	pcb_check_box_p -> setChecked (b);
}


bool ParamCheckBox :: SetValueFromText (const char *value_s)
{
	bool success_flag = false;

	if (value_s)
		{
			if (strcmp (value_s, "false") == 0)
				{
					pcb_check_box_p -> setChecked (false);
					success_flag = true;
				}
			else if (strcmp (value_s, "true") == 0)
				{
					pcb_check_box_p -> setChecked (true);
					success_flag = true;
				}
		}

	return success_flag;
}


bool ParamCheckBox :: SetValueFromJSON (const json_t * const value_p)
{
	bool success_flag = false;

	if (json_is_boolean (value_p))
		{
			if (value_p == json_true ())
				{
					pcb_check_box_p -> setChecked (true);
					success_flag = true;
				}
			else if (value_p == json_false ())
				{
					pcb_check_box_p -> setChecked (false);
					success_flag = true;
				}
		}

	return success_flag;
}


QWidget *ParamCheckBox :: GetQWidget ()
{
	return pcb_check_box_p;
}
