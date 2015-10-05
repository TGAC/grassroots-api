/*
** Copyright 2014-2015 The Genome Analysis Centre
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
#include <QLabel>
#include <QDesktopServices>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "qt_parameter_widget.h"
#include "file_chooser_widget.h"


#include "param_check_box.h"
#include "param_double_spin_box.h"
#include "param_spin_box.h"
#include "param_combo_box.h"
#include "param_line_edit.h"
#include "param_text_box.h"
#include "prefs_widget.h"
#include "param_table_widget.h"


// WHEATIS INCLUDES
#include "parameter.h"
#include "parameter_set.h"
#include "string_utils.h"
#include "provider.h"


const int QTParameterWidget :: QPW_NUM_COLUMNS = 2;


QTParameterWidget :: QTParameterWidget (const char *name_s, const char * const description_s, const char * const uri_s, const json_t *provider_p, ParameterSet *parameters_p, const PrefsWidget * const prefs_widget_p, const ParameterLevel initial_level)
:	qpw_params_p (parameters_p),
	qpw_prefs_widget_p (prefs_widget_p),
	qpw_widgets_map (QHash <Parameter *, BaseParamWidget *> ()),
	qpw_level (initial_level)
{
	QVBoxLayout *layout_p = new QVBoxLayout;
	QVBoxLayout *info_layout_p = new QVBoxLayout;

/*
	qpw_layout_p = new QGridLayout;
	qpw_layout_p -> setColumnStretch (1, 100);
	qpw_layout_p -> setAlignment (Qt :: AlignVCenter);
*/
	qpw_layout_p = new QFormLayout;
	qpw_layout_p -> setFieldGrowthPolicy(QFormLayout :: ExpandingFieldsGrow);
	layout_p -> addLayout (info_layout_p);

	setLayout (layout_p);

	QString str;

	str.append ("<b>");
	str.append (name_s);
	str.append ("</b>");

	QLabel *label_p = new QLabel (str, this);
	label_p -> setSizePolicy (QSizePolicy :: Fixed, QSizePolicy :: Fixed);
	info_layout_p -> addWidget (label_p);


	str.clear ();
	str.append (description_s);

	label_p = new QLabel (str, this);
	label_p -> setSizePolicy (QSizePolicy :: Fixed, QSizePolicy :: Fixed);
	info_layout_p -> addWidget (label_p);

	if (uri_s)
		{
			str.clear ();
			str.append ("For more information, go to <a href=\"");
			str.append (uri_s);
			str.append ("\">");
			str.append (uri_s);
			str.append ("</a>");

			label_p = new QLabel (str, this);
			connect (label_p,  &QLabel :: linkActivated, this, &QTParameterWidget :: OpenLink);

			label_p -> setSizePolicy (QSizePolicy :: Fixed, QSizePolicy :: Fixed);
			label_p -> setAlignment (Qt :: AlignCenter);
			info_layout_p -> addWidget (label_p);
		}

	if (provider_p)
		{
			const char *provider_name_s = GetProviderName (provider_p);

			if (provider_name_s)
				{
					const char *provider_uri_s = GetProviderURI (provider_p);

					str.clear ();
					str.append ("Provided by ");

					if (provider_uri_s)
						{
							str.append ("<a href=\"");
							str.append (provider_uri_s);
							str.append ("\">");
						}

					str.append (provider_name_s);

					if (provider_uri_s)
						{
							str.append ("</a>");
						}

					const char *provider_description_s = GetProviderDescription (provider_p);

					if (provider_description_s)
						{
							str.append (". ");
							str.append (provider_description_s);

							label_p = new QLabel (str, this);
							//label_p -> setWordWrap (true);
							connect (label_p,  &QLabel :: linkActivated, this, &QTParameterWidget :: OpenLink);
							label_p -> setSizePolicy (QSizePolicy :: Minimum, QSizePolicy :: Fixed);
							label_p -> setAlignment (Qt :: AlignLeft);
							//label_p -> setFrameShape (QFrame :: Box);

							info_layout_p -> addWidget (label_p);

						}
				}

		}		/* if (provider_p) */


	QFrame *line_p = new QFrame;
	line_p -> setFrameShape (QFrame :: HLine);
	QPalette palette = line_p -> palette ();
	palette.setColor (QPalette :: WindowText, Qt :: gray);
	line_p -> setPalette (palette);

	layout_p -> addWidget (line_p);

	layout_p -> insertSpacing (layout_p -> count (), 10);
	layout_p -> addLayout (qpw_layout_p);


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
/*
	qpw_layout_p -> addWidget (first_p, row, 0);
	qpw_layout_p -> addWidget (second_p, row, 1, row_span, 1, 0);
	second_p -> setSizePolicy (QSizePolicy :: Expanding, QSizePolicy :: Expanding);
*/
	qpw_layout_p -> addRow (first_p, second_p);
}


void QTParameterWidget :: AddParameters (ParameterSet *params_p)
{
	ParameterNode *node_p = reinterpret_cast <ParameterNode *> (params_p -> ps_params_p -> ll_head_p);
	ParameterGroupNode *param_group_node_p = reinterpret_cast <ParameterGroupNode *> (params_p -> ps_grouped_params_p -> ll_head_p);
	QHash <Parameter *, Parameter *> params_map;

	while (param_group_node_p)
		{
			ParameterGroup *group_p = param_group_node_p -> pgn_param_group_p;
			ParamGroupBox *box_p = new ParamGroupBox (group_p -> pg_name_s);
			Parameter **param_pp = group_p -> pg_params_pp;

			for (uint32 i = group_p -> pg_num_params; i > 0; -- i, ++ param_pp)
				{
					Parameter *param_p = const_cast <Parameter *> (*param_pp);

					AddParameterWidget (param_p, box_p);

					params_map.insert (param_p, param_p);
				}

			int row = qpw_layout_p -> rowCount ();
/*
			qpw_layout_p -> addWidget (box_p, row, 0, group_p -> pg_num_params, 2, Qt :: AlignVCenter);
			qpw_layout_p -> setColumnStretch (0, 100);
*/

			qpw_layout_p -> addRow (box_p);
			qpw_groupings.append (box_p);

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


void QTParameterWidget :: AddParameterWidget (Parameter *param_p, ParamGroupBox *group_p)
{
	BaseParamWidget *child_p = CreateWidgetForParameter (param_p);

	if (child_p)
		{
			if (group_p)
				{
					group_p -> AddParameterWidget (child_p);
					//group_p -> setSizePolicy (QSizePolicy :: Fixed, QSizePolicy :: Expanding);
				}
			else
				{
					AddRow (child_p -> GetLabel (), child_p -> GetUIQWidget (), 1);
				}

			qpw_widgets_map.insert (param_p, child_p);

			if (!CompareParameterLevels (param_p -> pa_level, qpw_level))
				{
					child_p -> SetVisible (false);
				}
		}


}




BaseParamWidget *QTParameterWidget :: GetWidgetForParameter (const char * const param_name_s) const
{
	BaseParamWidget *widget_p = 0;

	const QList <Parameter *> keys = qpw_widgets_map.keys ();

	for (int i = keys.size () - 1; i >= 0; -- i)
		{
			Parameter *param_p = keys.at (i);

			if (strcmp (param_p -> pa_name_s, param_name_s) == 0)
				{
					widget_p = qpw_widgets_map.value (param_p);
					i = -1;		/* force exit from loop */
				}
		}

	return widget_p;
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

			widget_p -> CheckLevelDisplay (level, parent_widget_p);
		}

	for (int i = qpw_groupings.count () - 1; i >= 0; -- i)
		{
			ParamGroupBox *box_p = qpw_groupings.at (i);
			box_p -> CheckVisibility (level);
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
					case PT_KEYWORD:
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

					case PT_CHAR:
						{
							ParamLineEdit *editor_p = new ParamLineEdit (param_p, qpw_prefs_widget_p, QLineEdit :: Normal);
							editor_p -> SetMaxLength (1);
							widget_p = editor_p;
						}
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
						widget_p = new ParamLineEdit (param_p, qpw_prefs_widget_p, QLineEdit :: Normal);
						break;

					case PT_PASSWORD:
						widget_p = new ParamTextBox (param_p, qpw_prefs_widget_p);
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

					case PT_LARGE_STRING:
					case PT_JSON:
						widget_p = new ParamTextBox (param_p, qpw_prefs_widget_p);
						break;

					case PT_TABLE:
						widget_p = new ParamTableWidget (param_p, qpw_prefs_widget_p);
						break;

				default:
						break;

				}		/* switch (param_p -> pa_type) */
		}

	if (widget_p)
		{
			QWidget *w_p = widget_p -> GetUIQWidget ();

			widget_p -> SetDefaultValue ();

			return widget_p;
		}		/* if (widget_p) */

	return NULL;
}


json_t *QTParameterWidget :: GetParameterValuesAsJSON () const
{
	/* make sure that all of the parameter values are up to date */
	QList <BaseParamWidget *> widgets = qpw_widgets_map.values ();

	for (int i = widgets.size () - 1; i >= 0; -- i)
		{
			BaseParamWidget *widget_p = widgets.at (i);

			if (! (widget_p -> StoreParameterValue ()))
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to set parameter value for %s", widget_p -> GetParameterName ());
				}
		}

	return GetParameterSetAsJSON (qpw_params_p, false);
}
