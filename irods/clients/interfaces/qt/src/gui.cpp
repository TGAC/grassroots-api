#include "gui.h"
#include "parameter.h"
#include "prefs_window.h"


int GetUserParameters (ParameterSet *params_set_p)
{
	int success_flag = 0;
	
	PrefsWindow *prefs_window_p = new PrefsWindow (params_set_p);
	
	if (prefs_window_p)
		{
			prefs_window_p ->  show ();
			
			
			delete prefs_window_p;
		}
		
	return success_flag;
}

