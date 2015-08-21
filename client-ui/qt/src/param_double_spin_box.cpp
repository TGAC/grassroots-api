#include <QDebug>

#include "param_double_spin_box.h"
#include "prefs_widget.h"
#include "math_utils.h"


ParamDoubleSpinBox :: ParamDoubleSpinBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
:		BaseParamWidget (param_p, options_widget_p)
{
	pdsb_spinner_p = new QDoubleSpinBox (parent_p);
	pdsb_spinner_p -> setDecimals (4);

	if (param_p -> pa_bounds_p)
		{
			pdsb_spinner_p -> setMinimum (param_p -> pa_bounds_p -> pb_lower.st_data_value);
			pdsb_spinner_p -> setMaximum (param_p -> pa_bounds_p -> pb_upper.st_data_value);
		}
	else
		{
			pdsb_spinner_p -> setMaximum (1000000.0);
		}
}


ParamDoubleSpinBox :: ~ParamDoubleSpinBox ()
{}


void ParamDoubleSpinBox :: SetDefaultValue ()
{
	pdsb_spinner_p -> setValue (bpw_param_p -> pa_default.st_data_value);
}


bool ParamDoubleSpinBox :: StoreParameterValue ()
{
	const double value = pdsb_spinner_p -> value ();
	bool b = SetParameterValue (bpw_param_p, &value);

	qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value;

	return b;
}


bool ParamDoubleSpinBox :: SetValueFromText (const char *value_s)
{
	bool success_flag  = false;
	double value;

	if (GetValidRealNumber (&value_s, &value, NULL))
		{
			pdsb_spinner_p -> setValue (value);
			success_flag = true;
		}

	return success_flag;
}




QWidget *ParamDoubleSpinBox :: GetQWidget ()
{
	return pdsb_spinner_p;
}
