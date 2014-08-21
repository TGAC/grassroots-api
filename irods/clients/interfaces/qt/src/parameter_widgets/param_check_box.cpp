#include "param_check_box.h"
#include "prefs_widget.h"


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
	bool widget_value = (state == Qt :: Checked) ? true : false;	
	return SetValue (&widget_value);
}


QWidget *ParamCheckBox :: GetQWidget ()
{
	return pcb_check_box_p;
}