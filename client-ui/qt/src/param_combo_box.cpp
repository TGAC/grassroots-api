#include "param_combo_box.h"
#include "prefs_widget.h"

#include "string_utils.h"


ParamComboBox :: ParamComboBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
: BaseParamWidget (param_p, options_widget_p)
{
	pcb_combo_box_p = new QComboBox (parent_p);

	void (QComboBox :: * signal_fn) (int) = &QComboBox :: currentIndexChanged;
	connect (pcb_combo_box_p, signal_fn, this, &ParamComboBox :: UpdateConfig);

	ParameterMultiOption *option_p = bpw_param_p -> pa_options_p -> pmoa_options_p;
	const int num_options = static_cast <int> (bpw_param_p -> pa_options_p -> pmoa_num_options);

	for (int i = 0; i < num_options; ++ i, ++ option_p)
		{
			char *option_s = option_p -> pmo_description_s;
			QVariant *v_p;

			switch (bpw_param_p -> pa_type)
				{
				case PT_STRING:
					v_p = new QVariant (option_p -> pmo_value.st_string_value_s);
					break;

					default:
					break;
				}

			if (v_p)
				{
					pcb_combo_box_p -> insertItem (i, QString (option_s), *v_p);
					delete v_p;
				}
		}


}


ParamComboBox ::	~ParamComboBox ()
{
	delete pcb_combo_box_p;
}


bool ParamComboBox :: UpdateConfig (int index)
{
	bool success_flag = false;
	QVariant v = pcb_combo_box_p -> itemData (index);

	if (bpw_param_p -> pa_type == PT_STRING)
		{
			QString s (v.toString ());
			QByteArray ba = s.toLocal8Bit ();


		}

	return success_flag;
}



QWidget *ParamComboBox :: GetQWidget ()
{
	return pcb_combo_box_p;
}
