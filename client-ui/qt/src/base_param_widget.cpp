#include "base_param_widget.h"

#include "string_utils.h"


BaseParamWidget	:: BaseParamWidget (Parameter * const param_p, const PrefsWidget * const prefs_widget_p)
	: bpw_param_p (param_p),
		bpw_prefs_widget_p (prefs_widget_p)
{
	bpw_param_name_s = CopyToNewString (param_p -> pa_name_s, 0, false);	
}


BaseParamWidget :: ~BaseParamWidget ()
{
	if (bpw_param_name_s)
		{
			FreeCopiedString (bpw_param_name_s);
		}
}



void BaseParamWidget :: RemoveConnection ()
{

}


void BaseParamWidget :: CheckLevelDisplay (const ParameterLevel ui_level, QWidget *sync_widget_p, const QWidget * const parent_widget_p)
{
	QWidget *this_widget_p = GetQWidget ();

	if (this_widget_p)
		{
			if (CompareParameterLevels (bpw_param_p -> pa_level, ui_level) > 0)
				{
					if (this_widget_p -> isVisibleTo (parent_widget_p))
						{
							this_widget_p -> hide ();

							if (sync_widget_p)
								{
									sync_widget_p -> hide ();
								}
						}
				}
			else
				{
					if (! (this_widget_p -> isVisibleTo (parent_widget_p)))
						{
							this_widget_p -> show ();

							if (sync_widget_p)
								{
									sync_widget_p -> show ();
								}
						}
				}
		}
}


