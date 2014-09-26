#include <QCheckBox>
#include <QHBoxLayout>
#include <QLayout>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

#include "service_prefs_widget.h"

ServicePrefsWidget::ServicePrefsWidget (const char * const service_name_s, const char * const service_description_s, ParameterSet *params_p, QWidget *parent_p)
: QWidget (parent_p),
	spw_service_name_s (service_name_s)
{
	QLayout *layout_p = new QVBoxLayout;
	spw_params_widget_p = new QTParameterWidget (service_name_s, service_description_s, params_p, NULL, PL_BASIC);

	layout_p -> addWidget (spw_params_widget_p);

	QString s ("Run ");
	s.append (service_name_s);

	QCheckBox *run_service_button_p = new QCheckBox (s, this);
	connect (run_service_button_p, &QCheckBox :: stateChanged, this, &ServicePrefsWidget :: SetRunFlag);

	QPushButton *reset_button_p = new QPushButton ("Restore Defaults", this);

	QHBoxLayout *buttons_layout_p = new QHBoxLayout;
	buttons_layout_p -> addWidget (run_service_button_p);
	buttons_layout_p -> addWidget (reset_button_p);

	layout_p -> addItem (buttons_layout_p);

	setLayout (layout_p);
}


void ServicePrefsWidget :: SetRunFlag (int state)
{
	spw_run_flag = (state == Qt :: Checked);
}


bool ServicePrefsWidget :: GetRunFlag () const
{
	return spw_run_flag;
}


json_t *ServicePrefsWidget :: GetServiceParamsAsJSON () const
{
	bool success_flag = true;

	json_t *service_json_p = json_object ();

	if (service_json_p)
		{
			if (spw_run_flag)
				{
					success_flag = (json_object_set_new (service_json_p, SERVICE_RUN_S, json_true ()) == 0);

					if (success_flag)
						{
							success_flag = (json_object_set_new (service_json_p, SERVICE_NAME_S, json_string (spw_service_name_s)) == 0);
						}

					if (success_flag)
						{
							json_t *params_json_p = spw_params_widget_p -> GetParameterValuesAsJSON ();

							if (params_json_p)
								{
									success_flag = (json_object_set_new (service_json_p, PARAM_SET_PARAMS_S, params_json_p) == 0);
								}
							else
								{
									success_flag = false;
								}

							/* Add each of the parameter values */
						}

				}		/* if (spw_run_flag) */
			else
				{
					success_flag = (json_object_set_new (service_json_p, SERVICE_RUN_S, json_false ()) == 0);
				}

		}		/* if (service_name_p) */

	if (!success_flag)
		{
			json_object_clear (service_json_p);
			json_decref (service_json_p);
		}

	return service_json_p;
}


