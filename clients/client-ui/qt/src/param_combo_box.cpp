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
#include <QVBoxLayout>

#include "param_combo_box.h"
#include "progress_window.h"

#include "prefs_widget.h"

#include "string_utils.h"
#include "math_utils.h"


ParamComboBox *ParamComboBox :: Create (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
{
	bool success_flag = true;
	ParamComboBox *combo_box_p = new ParamComboBox (param_p, options_widget_p, parent_p);

	if (combo_box_p -> bpw_param_p -> pa_options_p)
		{
			ParameterOptionNode *option_node_p = (ParameterOptionNode *) (combo_box_p -> bpw_param_p -> pa_options_p -> ll_head_p);
			bool loop_flag = (option_node_p != 0);

			while (loop_flag && success_flag)
				{
					ParameterOption *option_p = option_node_p -> pon_option_p;
					SharedType *current_value_p = & (param_p -> pa_current_value);

					if (combo_box_p -> AddOption (& (option_p -> po_value), option_p -> po_description_s, current_value_p))
						{
							option_node_p = (ParameterOptionNode *) (option_node_p -> pon_node.ln_next_p);

							loop_flag = (option_node_p != 0);
						}
					else
						{
							success_flag = false;
						}
				}		/* while (loop_flag && success_flag) */

		}		/* combo_box_p -> bpw_param_p -> pa_options_p */


	if (!success_flag)
		{
			delete combo_box_p;
			combo_box_p = 0;
		}

	return combo_box_p;
}


bool ParamComboBox :: AddOption (const SharedType *value_p, char *option_s, const SharedType *current_param_value_p)
{
	QVariant *v_p = 0;
	bool alloc_display_flag = false;
	bool success_flag = false;
	bool current_value_flag = false;

	switch (bpw_param_p -> pa_type)
		{
			case PT_STRING:
			case PT_KEYWORD:
			case PT_LARGE_STRING:
				{
					v_p = new QVariant (value_p -> st_string_value_s);

					if (current_param_value_p)
						{
							if (strcmp (value_p -> st_string_value_s, current_param_value_p -> st_string_value_s) == 0)
								{
									current_value_flag = true;
								}
						}

					if (!option_s)
						{
							option_s = value_p -> st_string_value_s;
						}
				}
				break;

			case PT_UNSIGNED_INT:
				v_p = new QVariant (value_p -> st_ulong_value);

				if (current_param_value_p)
					{
						if (value_p -> st_ulong_value == current_param_value_p -> st_ulong_value)
							{
								current_value_flag = true;
							}
					}

				if (!option_s)
					{
						option_s =	ConvertIntegerToString (value_p -> st_ulong_value);
						alloc_display_flag = (option_s != 0);
					}
				break;

		case PT_SIGNED_INT:
			v_p = new QVariant (value_p -> st_long_value);

			if (current_param_value_p)
				{
					if (value_p -> st_long_value == current_param_value_p -> st_long_value)
						{
							current_value_flag = true;
						}
				}

			if (!option_s)
				{
					option_s =	ConvertIntegerToString (value_p -> st_long_value);
					alloc_display_flag = (option_s != 0);
				}
			break;

		case PT_SIGNED_REAL:
		case PT_UNSIGNED_REAL:
			v_p = new QVariant (value_p -> st_data_value);

			if (current_param_value_p)
				{
					if (value_p -> st_data_value == current_param_value_p -> st_data_value)
						{
							current_value_flag = true;
						}
				}

			if (!option_s)
				{
					option_s =	ConvertDoubleToString (value_p -> st_data_value);
					alloc_display_flag = (option_s != 0);
				}
			break;

		default:
				break;
		}

	if (v_p && option_s)
		{
			QString display_str = QString (option_s);
			const int index = pcb_combo_box_p -> count ();
			pcb_combo_box_p -> insertItem (index, display_str, *v_p);

			if (current_value_flag)
				{
					pcb_combo_box_p -> setCurrentIndex (index);
				}

			success_flag = true;
		}

	if (alloc_display_flag)
		{
			FreeCopiedString (option_s);
		}

	if (v_p)
		{
			delete v_p;
		}

	return success_flag;
}

ParamComboBox :: ParamComboBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
: BaseParamWidget (param_p, options_widget_p), pcb_group_p (0)
{
	pcb_combo_box_p = new QComboBox (parent_p);
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
			case PT_KEYWORD:
				{
					QString s (v.toString ());
					QByteArray ba = s.toLocal8Bit ();

					const char *value_s = ba.constData ();

					success_flag = SetParameterValue (bpw_param_p, value_s, true);

					qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value_s;
				}
				break;

			case PT_CHAR:
				{
					QChar qc = v.toChar ();
					char c = qc.toLatin1 ();
					success_flag = SetParameterValue (bpw_param_p, &c, true);

					qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << c;
				}
				break;

			case PT_BOOLEAN:
				{
					bool b = v.toBool ();
					success_flag = SetParameterValue (bpw_param_p, &b, true);

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
							success_flag = SetParameterValue (bpw_param_p, &i, true);
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
							success_flag = SetParameterValue (bpw_param_p, &d, true);

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
					pcb_combo_box_p -> setCurrentText (value_s);
					success_flag  = true;
				}
				break;

			case PT_CHAR:
				{
					pcb_combo_box_p -> setCurrentText (value_s);
					success_flag  = true;
				}
				break;

			case PT_BOOLEAN:
				{
					if ((strcmp (value_s, "true") == 0) || ((strcmp (value_s, "false") == 0)))
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

					if (GetValidRealNumber (&value_s, &value, NULL))
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


bool ParamComboBox :: SetValueFromJSON (const json_t * const value_p)
{
	bool success_flag  = false;

	switch (bpw_param_p -> pa_type)
		{
			case PT_STRING:
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
			case PT_DIRECTORY:
				{
					if (json_is_string (value_p))
						{
							const char *value_s = json_string_value (value_p);

							pcb_combo_box_p -> setCurrentText (value_s);
							success_flag  = true;
						}
				}
				break;

			case PT_CHAR:
				{
					if (json_is_string (value_p))
						{
							const char *value_s = json_string_value (value_p);
							char c [2];

							*c = *value_s;
							* (c + 1) = '\0';

							pcb_combo_box_p -> setCurrentText (c);
							success_flag  = true;
						}
				}
				break;

			case PT_BOOLEAN:
				{
					if (json_is_true (value_p))
						{
							pcb_combo_box_p -> setCurrentText ("true");
						}
					else if (json_is_true (value_p))
						{
							pcb_combo_box_p -> setCurrentText ("false");
						}
				}
				break;

			case PT_SIGNED_INT:
			case PT_UNSIGNED_INT:
				{
					if (json_is_integer (value_p))
						{
							QString s;
							int value = json_integer_value (value_p);

							s.setNum (value);
							pcb_combo_box_p -> setCurrentText (s);

							success_flag = true;
						}
				}
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				{
					if (json_is_number (value_p))
						{
							QString s;
							double value = json_number_value (value_p);

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
