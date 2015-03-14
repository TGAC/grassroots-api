#include <QDebug>

#include "param_spin_box.h"
#include "prefs_widget.h"
#include "math_utils.h"


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
			psb_spin_box_p -> setMaximum (1000000);
		}

	void (QSpinBox :: * signal_fn) (int) = &QSpinBox :: valueChanged;
	connect (psb_spin_box_p, signal_fn, this, &ParamSpinBox :: UpdateConfig);
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


bool ParamSpinBox :: UpdateConfig (int value)
{
	bool b = SetParameterValue (bpw_param_p, &value);

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

