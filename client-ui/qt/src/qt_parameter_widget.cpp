#include <QLabel>
#include <QDesktopServices>

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


QTParameterWidget :: QTParameterWidget (const char *name_s, const char * const description_s, const char * const uri_s, ParameterSet *parameters_p, const PrefsWidget * const prefs_widget_p, const ParameterLevel initial_level)
:	qpw_params_p (parameters_p),
	qpw_prefs_widget_p (prefs_widget_p),
	qpw_widgets_map (QHash <Parameter *, BaseParamWidget *> ()),
	qpw_level (initial_level)
{
/*
	qpw_layout_p = new QFormLayout;
	qpw_layout_p -> setFormAlignment (Qt :: AlignVCenter);
	qpw_layout_p -> setLabelAlignment (Qt :: AlignVCenter);
*/

	qpw_layout_p = new QGridLayout;
	qpw_layout_p -> setAlignment (Qt :: AlignVCenter);

	setLayout (qpw_layout_p);

	QLabel *name_label_p = new QLabel (QString (name_s), this);
	QLabel *desc_label_p = new QLabel (QString (description_s), this);
	AddRow (name_label_p ,desc_label_p, 1);

			if (uri_s)
				{
					QString s ("For more information, go to <a href=\"");
					s.append (uri_s);
					s.append ("\">");
					s.append (uri_s);
					s.append ("</a>");

					name_label_p = new QLabel (s, this);
					connect (name_label_p,  &QLabel :: linkActivated, this, &QTParameterWidget :: OpenLink);
					qpw_layout_p -> addWidget (name_label_p, qpw_layout_p -> rowCount (), qpw_layout_p -> columnCount (), 1, 2, Qt :: AlignVCenter);
				}


	if (qpw_params_p)
		{
			AddParameters (qpw_params_p);
		}		/* if (parameters_p) */
}

void QTParameterWidget :: OpenLink (const QString &link_r)
{
	if (!QDesktopServices :: openUrl (QUrl (link_r)))
		{
			QWebView *browser_p = new QWebView;

			qpw_browsers.append (browser_p);
			browser_p -> load (QUrl (link_r));
			browser_p -> show ();
		}
}



void QTParameterWidget :: AddRow (QWidget *first_p, QWidget *second_p, const int row_span)
{
	int row = qpw_layout_p -> rowCount ();

	qpw_layout_p -> addWidget (first_p, row, 0);
	qpw_layout_p -> addWidget (second_p, row, 1, row_span, 1, 0);
}


void QTParameterWidget :: AddParameters (ParameterSet *params_p)
{
	ParameterNode *node_p = reinterpret_cast <ParameterNode *> (params_p -> ps_params_p -> ll_head_p);
	ParameterGroupNode *param_group_node_p = reinterpret_cast <ParameterGroupNode *> (params_p -> ps_grouped_params_p -> ll_head_p);
	QHash <Parameter *, Parameter *> params_map;

	while (param_group_node_p)
		{
			ParameterGroup *group_p = param_group_node_p -> pgn_param_group_p;
			QGroupBox *box_p = new QGroupBox (group_p -> pg_name_s);
			QFormLayout *layout_p = new QFormLayout;

			box_p -> setLayout (layout_p);
			box_p -> setAlignment (Qt:: AlignHCenter);

			const Parameter **param_pp = group_p -> pg_params_pp;

			for (uint32 i = group_p -> pg_num_params; i > 0; -- i, ++ param_pp)
				{
					Parameter *param_p = const_cast <Parameter *> (*param_pp);

					AddParameterWidget (param_p, layout_p);

					params_map.insert (param_p, param_p);
				}

			int row = qpw_layout_p -> rowCount ();
			qpw_layout_p -> addWidget (box_p, row, 0, group_p -> pg_num_params, 2, Qt :: AlignVCenter);

			param_group_node_p = reinterpret_cast <ParameterGroupNode *> (param_group_node_p -> pgn_node.ln_next_p);
		}

	while (node_p)
		{
			Parameter * const param_p = node_p -> pn_parameter_p;

			if (!params_map.contains (param_p))
				{
					AddParameterWidget (param_p);
				}

			node_p = reinterpret_cast <ParameterNode *> (node_p -> pn_node.ln_next_p);
		}		/* while (node_p) */

}


void QTParameterWidget :: AddParameterWidget (Parameter *param_p, QFormLayout *layout_p)
{
	BaseParamWidget *child_p = CreateWidgetForParameter (param_p);

	if (child_p)
		{
			QWidget *widget_p = child_p -> GetQWidget ();
			QLabel *label_p = new QLabel (GetUIName (param_p));

			if (layout_p)
				{
					layout_p -> addRow (label_p, widget_p);
				}
			else
				{
					AddRow (label_p, widget_p, 1);
				}

			qpw_widgets_map.insert (param_p, child_p);

			if (CompareParameterLevels (param_p -> pa_level, qpw_level) > 0)
				{
					widget_p -> hide ();
					label_p -> hide ();
				}
		}


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

	for (int i = qpw_browsers.size(); i > 0;-- i)
		{
			QWebView *browser_p = qpw_browsers.back ();
			qpw_browsers.pop_back ();
			delete browser_p;
		}
}



void QTParameterWidget :: ResetToDefaults ()
{
	QList <Parameter *> keys = qpw_widgets_map.keys ();

	for (int i = keys.size () - 1; i >= 0; --i)
		{
			Parameter *param_p = keys.at (i);
			BaseParamWidget *widget_p = qpw_widgets_map.value (param_p);
			widget_p -> SetDefaultValue ();
		}

}



void QTParameterWidget :: UpdateParameterLevel (const ParameterLevel level, const QWidget * const parent_widget_p)
{
	QHash <Parameter *, BaseParamWidget *> :: const_iterator i;

	for (i = qpw_widgets_map.constBegin (); i != qpw_widgets_map.constEnd (); ++ i)
		{
			BaseParamWidget *widget_p = reinterpret_cast <BaseParamWidget *> (i.value ());

			//widget_p -> CheckLevelDisplay (level, qpw_form_layout_p -> labelForField (widget_p -> GetQWidget ()), parent_widget_p);
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
					case PT_KEYWORD:
						widget_p = new ParamTextBox (param_p, qpw_prefs_widget_p, QLineEdit :: Normal);
						break;

					case PT_PASSWORD:
						widget_p = new ParamTextBox (param_p, qpw_prefs_widget_p, QLineEdit :: Password);
						break;

					case PT_SIGNED_INT:
						widget_p = new ParamSpinBox (param_p, qpw_prefs_widget_p, true);
						break;

					case PT_UNSIGNED_INT:
						widget_p = new ParamSpinBox (param_p, qpw_prefs_widget_p, false);
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

			widget_p -> SetDefaultValue ();

			return widget_p;
		}		/* if (widget_p) */

	return NULL;
}


json_t *QTParameterWidget :: GetParameterValuesAsJSON () const
{
	return GetParameterSetAsJSON (qpw_params_p, false);
}
