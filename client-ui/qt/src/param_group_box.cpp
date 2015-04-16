#include <QFormLayout>
#include "param_group_box.h"


ParamGroupBox :: ParamGroupBox (const char * const name_s)
 : QGroupBox (name_s)
{
	pg_layout_p = new QFormLayout;

	setLayout (pg_layout_p);
	setAlignment (Qt :: AlignRight);
}


void ParamGroupBox :: AddParameterWidget (BaseParamWidget *param_widget_p)
{
	QWidget *widget_p = param_widget_p -> GetQWidget ();
	QLabel *label_p = param_widget_p -> GetLabel ();

	pg_layout_p -> addRow (label_p, widget_p);
	pg_children.append (param_widget_p);
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
