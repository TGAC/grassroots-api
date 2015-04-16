#include "base_param_widget.h"

#include "string_utils.h"


BaseParamWidget	:: BaseParamWidget (Parameter * const param_p, const PrefsWidget * const prefs_widget_p)
	: bpw_param_p (param_p),
		bpw_prefs_widget_p (prefs_widget_p)
{
	bpw_param_name_s = CopyToNewString (param_p -> pa_name_s, 0, false);	
	bpw_label_p = new QLabel (GetUIName (param_p));
}


BaseParamWidget :: ~BaseParamWidget ()
{
	if (bpw_param_name_s)
		{
			FreeCopiedString (bpw_param_name_s);
		}
}


const char *BaseParamWidget :: GetParameterName () const
{
	return bpw_param_name_s;
}


void BaseParamWidget :: RemoveConnection ()
{

}


QLabel *BaseParamWidget :: GetLabel () const
{
	return bpw_label_p;
}


bool BaseParamWidget :: MeetsLevel (const ParameterLevel ui_level) const
{
	return CompareParameterLevels (bpw_param_p -> pa_level, ui_level);
}


void BaseParamWidget :: SetVisible (const bool visible_flag)
{
	QWidget *widget_p = GetQWidget ();

	if (widget_p)
		{
			widget_p -> setVisible (visible_flag);
		}

	bpw_label_p -> setVisible (visible_flag);
}


void BaseParamWidget :: CheckLevelDisplay (const ParameterLevel ui_level, const QWidget * const parent_widget_p)
{
	QWidget *this_widget_p = GetQWidget ();

	if (this_widget_p)
		{
			if (!CompareParameterLevels (bpw_param_p -> pa_level, ui_level))
				{
					if (this_widget_p -> isVisibleTo (parent_widget_p))
						{
							this_widget_p -> hide ();

							if (bpw_label_p)
								{
									bpw_label_p -> hide ();
								}
						}
				}
			else
				{
					if (! (this_widget_p -> isVisibleTo (parent_widget_p)))
						{
							this_widget_p -> show ();

							if (bpw_label_p)
								{
									bpw_label_p -> show ();
								}
						}
				}
		}
}


