#include <QLabel>


#include "qt_parameter_widget.h"
#include "file_chooser_widget.h"


#include "param_check_box.h"
#include "param_double_spin_box.h"
#include "param_spin_box.h"
#include "param_combo_box.h"
#include "param_text_box.h"

#include "prefs_widget.h"


// WHEATIS INCLUDES
#include "parameter.h"
#include "string_utils.h"


QTParameterWidget :: QTParameterWidget (const char *name_s, const char * const description_s, ParameterSet *parameters_p, const PrefsWidget * const prefs_widget_p, const ParameterLevel initial_level)
:	qpw_params_p (parameters_p),
	qpw_prefs_widget_p (prefs_widget_p),
	qpw_widgets_map (QHash <Parameter *, BaseParamWidget *> ()),
	qpw_level (initial_level)
{
	qpw_form_layout_p = new QFormLayout;
	qpw_form_layout_p -> setFormAlignment (Qt :: AlignVCenter);
	qpw_form_layout_p -> setLabelAlignment (Qt :: AlignVCenter);

	setLayout (qpw_form_layout_p);

	QLabel *label_p = new QLabel (QString (description_s), this);
	qpw_form_layout_p -> addRow (QString (name_s), label_p);

	if (qpw_params_p)
		{
			AddParameters (qpw_params_p);
		}		/* if (parameters_p) */
}


void QTParameterWidget :: AddParameters (ParameterSet *params_p)
{
	ParameterNode *node_p = reinterpret_cast <ParameterNode *> (params_p -> ps_params_p -> ll_head_p);

	while (node_p)
		{
			Parameter * const param_p = node_p -> pn_parameter_p;
			BaseParamWidget *child_p = CreateWidgetForParameter (param_p);

			if (child_p)
				{
					QWidget *widget_p = child_p -> GetQWidget ();

					qpw_form_layout_p -> addRow (param_p -> pa_name_s, widget_p);
					qpw_widgets_map.insert (param_p, child_p);

					if (CompareParameterLevels (param_p -> pa_level, qpw_level) > 0)
						{
							QWidget *label_p = qpw_form_layout_p -> labelForField (widget_p);

							widget_p -> hide ();
							label_p -> hide ();
						}
				}

			node_p = reinterpret_cast <ParameterNode *> (node_p -> pn_node.ln_next_p);
		}		/* while (node_p) */

}

QTParameterWidget :: ~QTParameterWidget ()
{
	QList <Parameter *> keys = qpw_widgets_map.keys ();

	while (!keys.isEmpty ())
		{
			Parameter *param_p = keys.takeLast ();

			BaseParamWidget *widget_p = qpw_widgets_map.take (param_p);

			widget_p -> RemoveConnection ();
		}
/*
	if (qpw_params_p)
		{
			FreeLinkedList (qpw_params_p);
		}
*/
}


void QTParameterWidget :: UpdateParameterLevel (const ParameterLevel level, const QWidget * const parent_widget_p)
{
	QHash <Parameter *, BaseParamWidget *> :: const_iterator i;

	for (i = qpw_widgets_map.constBegin (); i != qpw_widgets_map.constEnd (); ++ i)
		{
			BaseParamWidget *widget_p = reinterpret_cast <BaseParamWidget *> (i.value ());

			widget_p -> CheckLevelDisplay (level, qpw_form_layout_p -> labelForField (widget_p -> GetQWidget ()), parent_widget_p);
		}

	qpw_level = level;
}




BaseParamWidget *QTParameterWidget :: CreateWidgetForParameter (Parameter * const param_p)
{
	BaseParamWidget *widget_p = NULL;

	if (param_p -> pa_options_p)
		{
			switch (param_p -> pa_type)
				{
					case PT_STRING:
						widget_p = new ParamComboBox (param_p, qpw_prefs_widget_p);
						break;

					default:
						break;
				}
		}
	else
		{
			switch (param_p -> pa_type)
				{
					case PT_BOOLEAN:
						widget_p = new ParamCheckBox (param_p, qpw_prefs_widget_p);
						break;

					case PT_SIGNED_REAL:
					case PT_UNSIGNED_REAL:
						widget_p = new ParamDoubleSpinBox (param_p, qpw_prefs_widget_p);
						break;

					case PT_FILE_TO_READ:
						widget_p = new FileChooserWidget (param_p, qpw_prefs_widget_p, QFileDialog :: ExistingFile);
						break;

					case PT_FILE_TO_WRITE:
						widget_p = new FileChooserWidget (param_p, qpw_prefs_widget_p, QFileDialog :: AnyFile);
						break;

					case PT_STRING:
						widget_p = new ParamTextBox (param_p, qpw_prefs_widget_p);
						break;

					case PT_SIGNED_INT:
					case PT_UNSIGNED_INT:
						widget_p = new ParamSpinBox (param_p, qpw_prefs_widget_p);
						break;

					case PT_DIRECTORY:
						widget_p = new FileChooserWidget (param_p, qpw_prefs_widget_p, QFileDialog :: Directory);
						break;

				default:
						break;

				}		/* switch (param_p -> pa_type) */
		}

	if (widget_p)
		{
			QWidget *w_p = widget_p -> GetQWidget ();

			w_p -> setToolTip (param_p -> pa_description_s);

			return widget_p;
		}		/* if (widget_p) */

	return NULL;
}


json_t *QTParameterWidget :: GetParameterValuesAsJSON () const
{
	return GetParameterSetAsJSON (qpw_params_p, false);
}
