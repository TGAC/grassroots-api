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
#include "services_list.h"
#include "services_tabs.h"

#include "service.h"
#include "string_utils.h"

using namespace std;



PrefsWidget :: PrefsWidget (QWidget *parent_p,  ParameterLevel initial_level, const bool tabbed_display_flag)
:	QWidget (parent_p),
	pw_level (initial_level)
{
	QVBoxLayout *layout_p = new QVBoxLayout;

	if (tabbed_display_flag)
		{
			pw_services_ui_p = new ServicesTabs (this);
		}
	else
		{
			pw_services_ui_p = new ServicesList (this);
		}

	layout_p -> addWidget (pw_services_ui_p -> GetWidget ());


	QHBoxLayout *buttons_layout_p = new QHBoxLayout;
	QPushButton *ok_button_p = new QPushButton (QIcon ("images/run"), tr ("Run"), this);
	QPushButton *cancel_button_p = new QPushButton (QIcon ("images/cancel"), tr ("Quit"), this);

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
	emit RunServices (true);
}


void PrefsWidget :: Reject ()
{
	emit RunServices (false);
}


void PrefsWidget :: CreateAndAddServicePage (const json_t * const service_json_p)
{
	const char *service_name_s = GetServiceNameFromJSON (service_json_p);

	if (service_name_s)
		{
			const char *service_description_s = GetServiceDescriptionFromJSON (service_json_p);

			if (service_description_s)
				{
					ParameterSet *params_p = CreateParameterSetFromJSON (service_json_p);

					if (params_p)
						{
							const char *service_info_uri_s = GetOperationInformationURIFromJSON (service_json_p);

							CreateAndAddServicePage (service_name_s, service_description_s, service_info_uri_s, params_p);
						}		/* if (params_p) */

				}		/* if (service_description_s) */

		}		/* if (service_name_s) */
}


void PrefsWidget :: CreateAndAddServicePage (const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, ParameterSet *params_p)
{
	ServicePrefsWidget *service_widget_p = new ServicePrefsWidget (service_name_s, service_description_s, service_info_uri_s, params_p, this);

	pw_services_ui_p -> AddService (service_name_s, service_widget_p);
	pw_service_widgets.append (service_widget_p);
}


bool PrefsWidget :: SetServiceParams (json_t *services_config_p)
{
	bool success_flag = false;

	if (json_is_array (services_config_p))
		{
			json_t *service_config_p;
			size_t i;

			json_array_foreach (services_config_p, i, service_config_p)
				{
					const char *service_name_s = GetJSONString (service_config_p, SERVICES_NAME_S);

					if (service_name_s)
						{
							ServicePrefsWidget *service_widget_p = 0;

							/* find the service widget */
							for (int i = pw_service_widgets.size () - 1; i >= 0; -- i)
								{
									ServicePrefsWidget *widget_p = pw_service_widgets.at (i);

									if (strcmp (widget_p -> GetServiceName (), service_name_s) == 0)
										{
											service_widget_p = widget_p;
											i = -1;		// force exit from loop
										}
								}

							if (service_widget_p)
								{
									service_widget_p -> SetServiceParams (service_config_p);
								}		/* if (service_widget_p) */

						}		/* if (service_name_s) */
				}

			success_flag = true;
		}

	return success_flag;

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


json_t *PrefsWidget :: GetUserValuesAsJSON (bool full_flag) const
{
	json_t *root_p = json_array ();

	if (root_p)
		{
			const int num_services = pw_service_widgets.size ();
			int i = 0;
			bool success_flag = true;

			char *dump_s = json_dumps (root_p, 0);

			if (dump_s)
				{
					free (dump_s);
				}

			while (success_flag && (i < num_services))
				{
					ServicePrefsWidget *spw_p = pw_service_widgets.at (i);
					json_t *service_json_p = spw_p -> GetServiceParamsAsJSON (full_flag);

					if (service_json_p)
						{
							dump_s = json_dumps (service_json_p, 0);
							if (dump_s)
								{
									free (dump_s);
								}

							success_flag = (json_array_append_new (root_p, service_json_p) == 0);

							dump_s = json_dumps (root_p, 0);
							if (dump_s)
								{
									free (dump_s);
								}
						}

					if (success_flag)
						{
							++ i;
						}
				}

		}		/* if (root_p) */

	char *client_results_s = json_dumps (root_p, 0);
	size_t l = json_array_size (root_p);
	for (size_t i = 0; i < l; ++ i)
		{
			json_t *json_p = json_array_get (root_p, i);
			char *dump_s = json_dumps (json_p, 0);

			if (dump_s)
				{
					free (dump_s);
				}
		}

	if (client_results_s)
		{
			free (client_results_s);
		}

	return root_p;
}


