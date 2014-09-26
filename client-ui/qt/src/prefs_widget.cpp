#include <iostream>

#include <QLayout>
#include <QGroupBox>
#include <QAction>
#include <QCheckBox>
#include <QMenuBar>
#include <QMenu>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QString>
#include <QTabWidget>

#include "prefs_widget.h"

#include "file_chooser_widget.h"
#include "qt_parameter_widget.h"


#include "service.h"


using namespace std;



PrefsWidget :: PrefsWidget (QWidget *parent_p,  ParameterLevel initial_level)
    :	QWidget (parent_p),
		pw_level (initial_level)
{
	pw_tabs_p = new QTabWidget;

	QVBoxLayout *layout_p = new QVBoxLayout;
	layout_p -> addWidget (pw_tabs_p);

	QHBoxLayout *buttons_layout_p = new QHBoxLayout;
	QPushButton *ok_button_p = new QPushButton (tr ("Ok"), this);
	QPushButton *cancel_button_p = new QPushButton (tr ("Cancel"), this);

	buttons_layout_p -> addWidget (ok_button_p);
	buttons_layout_p -> addWidget (cancel_button_p);

	layout_p -> addItem (buttons_layout_p);

	setLayout (layout_p);

	connect (ok_button_p, &QAbstractButton :: clicked, 	this, &PrefsWidget :: Accept);
	connect (cancel_button_p, &QAbstractButton :: clicked, 	this, &PrefsWidget :: Reject);
}


PrefsWidget :: ~PrefsWidget ()
{
}


void PrefsWidget :: Accept ()
{
	emit Finished (QDialog :: Accepted);
}


void PrefsWidget :: Reject ()
{
	emit Finished (QDialog :: Rejected);
}


void PrefsWidget :: CreateAndAddServicePage (const json_t * const service_json_p)
{
	const char *service_name_s = GetServiceNameFromJSON (service_json_p);

	if (service_name_s)
		{
			const char *service_description_s = GetServiceNameFromJSON (service_json_p);

			if (service_description_s)
				{
					ParameterSet *params_p = CreateParameterSetFromJSON (service_json_p);

					if (params_p)
						{
							CreateAndAddServicePage (service_name_s, service_description_s, params_p);
						}		/* if (params_p) */

				}		/* if (service_description_s) */

		}		/* if (service_name_s) */
}


void PrefsWidget :: CreateAndAddServicePage (const char * const service_name_s, const char * const service_description_s, ParameterSet *params_p)
{
	ServicePrefsWidget *service_widget_p = new ServicePrefsWidget (service_name_s, service_description_s, params_p, this);

	pw_tabs_p -> addTab (service_widget_p, QString (service_name_s));
	pw_service_widgets.append (service_widget_p);
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


json_t *PrefsWidget :: GetUserValuesAsJSON () const
{
	json_t *root_p = json_array ();

	if (root_p)
		{
			const int num_services = pw_service_widgets.size ();
			int i = 0;
			bool success_flag = true;

			while (success_flag && (i < num_services))
				{
					ServicePrefsWidget *spw_p = pw_service_widgets.at (i);
					json_t *service_json_p = spw_p -> GetServiceParamsAsJSON ();

					if (service_json_p)
						{
							success_flag = (json_array_append_new (root_p, service_json_p) == 0);
						}

					if (success_flag)
						{
							++ i;
						}
				}

		}		/* if (root_p) */


	return root_p;
}


