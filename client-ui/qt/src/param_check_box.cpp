#include <QDebug>

#include "param_check_box.h"

#include "progress_window.h"
#include "prefs_widget.h"

#include "string_utils.h"


ParamCheckBox :: ParamCheckBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
: BaseParamWidget (param_p, options_widget_p)
{
	pcb_check_box_p = new QCheckBox (parent_p);

	void (QCheckBox :: * signal_fn) (int) = &QCheckBox :: stateChanged;
	connect (pcb_check_box_p, signal_fn, this, &ParamCheckBox :: UpdateConfig);
}


ParamCheckBox ::	~ParamCheckBox ()
{
	delete pcb_check_box_p;
}


bool ParamCheckBox :: UpdateConfig (int state)
{
	bool b = false;
	bool value = false;

	switch (state)
		{
			case Qt::Checked:
				{
					value = true;
				}
				break;

			case Qt :: Unchecked:
				{
					value = false;
				}
				break;

			default:
				break;
		}

	b = SetParameterValue (bpw_param_p, &value);

	qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value;

	return b;
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



QWidget *ParamCheckBox :: GetQWidget ()
{
	return pcb_check_box_p;
}
