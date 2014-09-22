#include <iostream>

#include <QLayout>
#include <QGroupBox>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QTabWidget>

#include "prefs_widget.h"

#include "file_chooser_widget.h"
#include "qt_parameter_widget.h"


using namespace std;



PrefsWidget :: PrefsWidget (QWidget *parent_p,  ParameterLevel initial_level)
    :	QWidget (parent_p),
		pw_level (initial_level)
{
	QTabWidget *tabs_p = new QTabWidget;

	QHBoxLayout *layout_p = new QHBoxLayout;
	layout_p -> addWidget (tabs_p);
	setLayout (layout_p);
}


PrefsWidget :: ~PrefsWidget ()
{
}


void PrefsWidget :: AddServicePage (QTabWidget *tab_p, const json_t * const service_json_p)
{
	QWidget *page_p = new QWidget;
	QLayout *layout_p = new QVBoxLayout;

	const char *service_name_s = GetServiceNameFromJSON (service_json_p);

	if (service_name_s)
		{
			const char *service_description_s = GetServiceNameFromJSON (service_json_p);

			if (service_description_s)
				{
						ParameterSet *params_p = CreateParameterSetFromJSON (service_json_p);

						if (params_p)
							{
								QTParameterWidget *widget_p = new QTParameterWidget (service_name_s, service_description_s, params_p, NULL, PL_BASIC);

								layout_p -> addWidget (widget_p);
								page_p -> setLayout (layout_p);

								tab_p -> addTab (page_p, QString (service_name_s));


							}		/* if (params_p) */

				}		/* if (service_description_s) */
		}		/* if (service_name_s) */
}


ParameterLevel PrefsWidget :: GetCurrentParameterLevel () const
{
	return pw_level;
}


void PrefsWidget :: SetInterfaceLevel (ParameterLevel level)
{
	if (pw_level != level)
		{
			emit InterfaceLevelChanged (level);
			pw_level = level;
		}		/* if (pw_level != level) */
}



void PrefsWidget :: ShowServiceConfigurationWidget (Service *service_p)
{
	if (service_p)
		{
			const char * const name_s = service_p -> se_get_service_name_fn ();
			const char * const description_s = service_p -> se_get_service_description_fn ();
			ParameterSet *params_p = service_p -> se_get_params_fn (service_p -> se_data_p);

			if (params_p)
				{
					QTParameterWidget *widget_p = new QTParameterWidget (name_s, description_s, params_p, this, pw_level);

					if (widget_p)
						{
							layout()->addWidget (widget_p);

						}		/* if (widget_p) */

					FreeParameterSet (params_p);
				}		/* if (params_p) */

		}		/* if (service_p) */

}

