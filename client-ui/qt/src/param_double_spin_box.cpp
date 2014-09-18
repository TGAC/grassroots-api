#include "param_double_spin_box.h"
#include "prefs_widget.h"

#include "math_utils.h"
#include "string_utils.h"


ParamDoubleSpinBox :: ParamDoubleSpinBox (const Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
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


ParamDoubleSpinBox ::	~ParamDoubleSpinBox ()
{}






QWidget *ParamDoubleSpinBox :: GetQWidget ()
{
	return pdsb_spinner_p;
}
