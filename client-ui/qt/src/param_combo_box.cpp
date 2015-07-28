#include <QDebug>
#include <QVBoxLayout>

#include "param_combo_box.h"
#include "progress_window.h"

#include "prefs_widget.h"

#include "string_utils.h"
#include "math_utils.h"


ParamComboBox :: ParamComboBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
: BaseParamWidget (param_p, options_widget_p), pcb_group_p (0)
{
	pcb_combo_box_p = new QComboBox (parent_p);


	ParameterMultiOption *option_p = bpw_param_p -> pa_options_p -> pmoa_options_p;
	const int num_options = static_cast <int> (bpw_param_p -> pa_options_p -> pmoa_num_options);

	for (int i = 0; i < num_options; ++ i, ++ option_p)
		{
			char *option_s = option_p -> pmo_description_s;
			QString display_str;
			QVariant *v_p;

			switch (bpw_param_p -> pa_type)
				{
				case PT_STRING:
					v_p = new QVariant (option_p -> pmo_value.st_string_value_s);
					if (!option_s)
						{
							option_s =	option_p -> pmo_value.st_string_value_s;
						}
					display_str = QString (option_s);
					break;

					default:
					break;
				}

			if (v_p)
				{
					pcb_combo_box_p -> insertItem (i, display_str, *v_p);
					delete v_p;
				}
		}

/*
	const char *title_s = param_p -> pa_display_name_s ;

	if (!title_s)
		{
			title_s = param_p -> pa_name_s;
		}

	pcb_group_p = new QGroupBox (title_s);
	QVBoxLayout *layout_p = new QVBoxLayout;

	layout_p -> addWidget (pcb_combo_box_p);
	pcb_group_p -> setLayout (layout_p);
*/
}


ParamComboBox ::	~ParamComboBox ()
{
	delete pcb_combo_box_p;
}


bool ParamComboBox :: StoreParameterValue ()
{
	QVariant v = pcb_combo_box_p -> currentData ();
	bool success_flag = false;

	switch (bpw_param_p -> pa_type)
		{
			case PT_LARGE_STRING:
			case PT_STRING:
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
			case PT_DIRECTORY:
				{
					QString s (v.toString ());
					QByteArray ba = s.toLocal8Bit ();

					const char *value_s = ba.constData ();

					success_flag = SetParameterValue (bpw_param_p, value_s);

					qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value_s;
				}
				break;

			case PT_CHAR:
				{
					QChar qc = v.toChar ();
					char c = qc.toLatin1 ();
					success_flag = SetParameterValue (bpw_param_p, &c);

					qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << c;
				}
				break;

			case PT_BOOLEAN:
				{
					bool b = v.toBool ();
					success_flag = SetParameterValue (bpw_param_p, &b);

					qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << b;
				}
				break;

			case PT_SIGNED_INT:
			case PT_UNSIGNED_INT:
				{
					bool conv_flag = false;
					int i = v.toInt (&conv_flag);

					if (conv_flag)
						{
							success_flag = SetParameterValue (bpw_param_p, &i);
							qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << i;
						}
				}
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				{
					bool conv_flag = false;
					double d = v.toDouble (&conv_flag);

					if (conv_flag)
						{
							success_flag = SetParameterValue (bpw_param_p, &d);

							qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << d;
						}
				}
				break;

			default:
				break;
		}


	return success_flag;
}



bool ParamComboBox :: SetValueFromText (const char *value_s)
{
	bool success_flag  = false;

	switch (bpw_param_p -> pa_type)
		{
			case PT_STRING:
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
			case PT_DIRECTORY:
				{
					pcb_combo_box_p -> setCurrentText (bpw_param_p -> pa_default.st_string_value_s);
					success_flag  = true;
				}
				break;

			case PT_CHAR:
				{
					pcb_combo_box_p -> setCurrentText (& (bpw_param_p -> pa_default.st_char_value));
					success_flag  = true;
				}
				break;

			case PT_BOOLEAN:
				{
					if ((strcmp (value_s, "true") == 0) || ((strcmp (value_s, "true") == 0)))
						{
							pcb_combo_box_p -> setCurrentText (value_s);
							success_flag  = true;
						}
				}
				break;

			case PT_SIGNED_INT:
			case PT_UNSIGNED_INT:
				{
					int32 value;

					if (GetValidInteger (&value_s, &value))
						{
							QString s;

							s.setNum (value);
							pcb_combo_box_p -> setCurrentText (s);

							success_flag = true;
						}
				}
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				{
					double value;

					if (GetValidRealNumber (&value_s, &value))
						{
							QString s;

							s.setNum (value);
							pcb_combo_box_p -> setCurrentText (s);

							success_flag = true;
						}
				}
				break;

			default:
				break;
		}

	return success_flag;
}



void ParamComboBox :: SetDefaultValue ()
{
	switch (bpw_param_p -> pa_type)
		{
			case PT_STRING:
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
			case PT_DIRECTORY:
				{
					pcb_combo_box_p -> setCurrentText (bpw_param_p -> pa_default.st_string_value_s);
				}
				break;

			case PT_CHAR:
				{
					pcb_combo_box_p -> setCurrentText (& (bpw_param_p -> pa_default.st_char_value));
				}
				break;

			case PT_BOOLEAN:
				{
					const char *value_s = (bpw_param_p -> pa_default.st_boolean_value) ? "True" : "False";
					pcb_combo_box_p -> setCurrentText (value_s);
				}
				break;

			case PT_SIGNED_INT:
				{
					QString s;

					s.setNum (bpw_param_p -> pa_default.st_long_value);
					pcb_combo_box_p -> setCurrentText (s);
				}
				break;


			case PT_UNSIGNED_INT:
				{
					QString s;

					s.setNum (bpw_param_p -> pa_default.st_ulong_value);
					pcb_combo_box_p -> setCurrentText (s);
				}
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				{
					QString s;

					s.setNum (bpw_param_p -> pa_default.st_data_value);
					pcb_combo_box_p -> setCurrentText (s);
				}
				break;

			default:
				break;
		}
}


QWidget *ParamComboBox :: GetQWidget ()
{
	return pcb_combo_box_p;
}
