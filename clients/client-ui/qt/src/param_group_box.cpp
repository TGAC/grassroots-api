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
#include <QFormLayout>
#include "param_group_box.h"


ParamGroupBox :: ParamGroupBox (const char * const name_s, const bool visible_flag)
 : QGroupBox (name_s)
{
	pg_layout_p = new QFormLayout;

	setStyleSheet("QGroupBox { font-weight: bold; } ");

	setCheckable (true);
	setChecked (visible_flag);
	setLayout (pg_layout_p);
	setAlignment (Qt :: AlignLeft);

	connect (this, &ParamGroupBox :: clicked, this, &ParamGroupBox :: ToggleCollapsed);
}


void ParamGroupBox :: AddParameterWidget (BaseParamWidget *param_widget_p)
{
	QWidget *widget_p = param_widget_p -> GetUIQWidget ();
	QLabel *label_p = param_widget_p -> GetLabel ();

	pg_layout_p -> addRow (label_p, widget_p);
	pg_children.append (param_widget_p);

	if (! (isChecked ()))
		{
			param_widget_p -> SetVisible (false);
		}
}


void ParamGroupBox :: paintEvent (QPaintEvent *event_p)
{
	QGroupBox :: paintEvent (event_p);
}


void ParamGroupBox :: ToggleCollapsed (bool checked)
{
	if (checked)
		{
			for (int j = pg_children.count () - 1; j >= 0; -- j)
				{
					BaseParamWidget *widget_p = pg_children.at (j);

					widget_p -> SetVisible (true);
				}

		}
	else
		{
			for (int j = pg_children.count () - 1; j >= 0; -- j)
				{
					BaseParamWidget *widget_p = pg_children.at (j);

					widget_p -> SetVisible (false);
				}
		}
}


void ParamGroupBox :: CheckVisibility (ParameterLevel level)
{
	if (isHidden ())
		{
			bool any_visible_children_flag = false;

			for (int j = pg_children.count () - 1; j >= 0; -- j)
				{
					BaseParamWidget *widget_p = pg_children.at (j);

					if (widget_p -> MeetsLevel (level))
						{
							any_visible_children_flag = true;
							j = 0;
						}
				}

			if (any_visible_children_flag)
				{
					show ();
				}
		}
	else
		{
			bool any_visible_children_flag = false;

			for (int j = pg_children.count () - 1; j >= 0; -- j)
				{
					BaseParamWidget *widget_p = pg_children.at (j);

					if (widget_p -> MeetsLevel (level))
						{
							any_visible_children_flag = true;
							j = 0;
						}
				}

			if (!any_visible_children_flag)
				{
					hide ();
				}
		}
}
