#include <QDebug>
#include <QFormLayout>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "string_utils.h"
#include "service_prefs_widget.h"


QTParameterWidget *ServicePrefsWidget :: GetServiceWidget (Service *service_p)
{
	QTParameterWidget *widget_p = NULL;

	if (service_p)
		{
				const char *module_name_s = service_p -> se_get_service_name_fn ();
				const char *description_s = service_p -> se_get_service_description_fn ();
				ParameterSet *params_p = service_p -> se_get_params_fn (service_p -> se_data_p);
				const ParameterLevel initial_level = PL_BASIC;

			if (params_p)
			{
				widget_p = new QTParameterWidget (module_name_s, description_s, params_p, NULL, initial_level);
			}

		}		/* if (service_p) */

	return widget_p;
}


ServicePrefsWidget :: ~ServicePrefsWidget ()
{

}

