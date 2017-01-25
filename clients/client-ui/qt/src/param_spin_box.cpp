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

#include "param_spin_box.h"
#include "prefs_widget.h"
#include "math_utils.h"


int ParamSpinBox :: PSB_DEFAULT_MIN = INT32_MIN;

int ParamSpinBox :: PSB_DEFAULT_MAX = INT32_MAX;


ParamSpinBox :: ParamSpinBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, bool signed_flag, QWidget *parent_p)
:	BaseParamWidget (param_p, options_widget_p),
	psb_signed_flag (signed_flag)
{
	psb_spin_box_p = new QSpinBox (parent_p);

	if (param_p -> pa_bounds_p)
		{
			psb_spin_box_p -> setMinimum (param_p -> pa_bounds_p -> pb_lower.st_long_value);
			psb_spin_box_p -> setMaximum (param_p -> pa_bounds_p -> pb_upper.st_long_value);
		}
	else
		{
			switch (param_p -> pa_type)
				{
					case PT_NEGATIVE_INT:
						psb_spin_box_p -> setRange (ParamSpinBox :: PSB_DEFAULT_MIN, 0);
						break;

					case PT_UNSIGNED_INT:
						psb_spin_box_p -> setRange (0, ParamSpinBox :: PSB_DEFAULT_MAX);
						break;

					case PT_SIGNED_INT:
						psb_spin_box_p -> setRange (ParamSpinBox :: PSB_DEFAULT_MIN, ParamSpinBox :: PSB_DEFAULT_MAX);
						break;

					default:
						break;
				}
		}

}


ParamSpinBox ::	~ParamSpinBox ()
{
	delete psb_spin_box_p;
}


QWidget *ParamSpinBox :: GetQWidget ()
{
	return psb_spin_box_p;
}


void ParamSpinBox :: SetDefaultValue ()
{
	if (psb_signed_flag)
		{
			psb_spin_box_p -> setValue (bpw_param_p -> pa_default.st_long_value);
		}
	else
		{
			psb_spin_box_p -> setValue (bpw_param_p -> pa_default.st_ulong_value);
		}
}

bool ParamSpinBox :: StoreParameterValue ()
{
	const int value = psb_spin_box_p -> value ();
	bool b = SetParameterValue (bpw_param_p, &value, true);

	qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value;

	return b;
}


bool ParamSpinBox :: SetValueFromText (const char *value_s)
{
	bool success_flag  = true;
	int value;

	if (GetValidInteger (&value_s, &value))
		{
			psb_spin_box_p -> setValue (value);
			success_flag = true;
		}

	return success_flag;
}


bool ParamSpinBox :: SetValueFromJSON (const json_t * const value_p)
{
	bool success_flag = false;

	if (json_is_integer (value_p))
		{
			const int d = json_integer_value (value_p);

			psb_spin_box_p -> setValue (d);
			success_flag = true;
		}

	return success_flag;
}

