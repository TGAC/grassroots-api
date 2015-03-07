#include <QCheckBox>
#include <QHBoxLayout>
#include <QLayout>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QIcon>

#include "service_prefs_widget.h"

#ifdef _DEBUG
	#define SERVICE_PREFS_WIDGET_DEBUG (DEBUG_FINE)
#else
	#define SERVICE_PREFS_WIDGET_DEBUG (DEBUG_NONE)
#endif

ServicePrefsWidget::ServicePrefsWidget (const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s,  ParameterSet *params_p, QWidget *parent_p)
: QWidget (parent_p),
	spw_service_name_s (service_name_s)
{
	QLayout *layout_p = new QVBoxLayout;

	spw_run_flag = false;

	spw_params_widget_p = new QTParameterWidget (service_name_s, service_description_s, service_info_uri_s, params_p, NULL, PL_BASIC);
	layout_p -> addWidget (spw_params_widget_p);

	QString s ("Run ");
	s.append (service_name_s);

	QCheckBox *run_service_button_p = new QCheckBox (s, this);
	run_service_button_p -> setChecked (spw_run_flag);
	connect (run_service_button_p, &QCheckBox :: stateChanged, this, &ServicePrefsWidget :: SetRunFlag);

	QPushButton *reset_button_p = new QPushButton (QIcon ("images/reload"), "Restore Defaults", this);
	connect (reset_button_p, &QAbstractButton :: clicked, spw_params_widget_p, &QTParameterWidget :: ResetToDefaults);

	QHBoxLayout *buttons_layout_p = new QHBoxLayout;
	buttons_layout_p -> addWidget (run_service_button_p);
	buttons_layout_p -> addWidget (reset_button_p);

	layout_p -> addItem (buttons_layout_p);

	setLayout (layout_p);
}


ServicePrefsWidget :: ~ServicePrefsWidget ()
{

}


void ServicePrefsWidget :: SetRunFlag (int state)
{
	spw_run_flag = (state == Qt :: Checked);
}


bool ServicePrefsWidget :: GetRunFlag () const
{
	return spw_run_flag;
}

json_t *ServicePrefsWidget :: GetServiceParamsAsJSON (bool full_flag) const
{
	bool success_flag = true;
	json_t *service_json_p = json_object ();

	if (service_json_p)
		{
			success_flag = (json_object_set_new (service_json_p, SERVICES_NAME_S, json_string (spw_service_name_s)) == 0) && (json_object_set_new (service_json_p, SERVICE_RUN_S, spw_run_flag ? json_true () : json_false ()) == 0);

			if (success_flag)
				{
					if (spw_run_flag || full_flag)
						{
							#if SERVICE_PREFS_WIDGET_DEBUG >= DEBUG_FINE
							PrintJSON (stdout, service_json_p, "1 >>\n");
							#endif

							json_t *param_set_json_p = spw_params_widget_p -> GetParameterValuesAsJSON ();

							#if SERVICE_PREFS_WIDGET_DEBUG >= DEBUG_FINE
							PrintJSON (stdout, param_set_json_p, "3 >>\n");
							#endif

							success_flag = false;

							if (param_set_json_p)
								{
									if (json_object_set_new (service_json_p, PARAM_SET_KEY_S, param_set_json_p) == 0)
										{
											success_flag = true;
										}
									else
										{
											json_object_clear (param_set_json_p);
											json_decref (param_set_json_p);
											param_set_json_p = NULL;
										}
								}

							#if SERVICE_PREFS_WIDGET_DEBUG >= DEBUG_FINE
							PrintJSON (stdout, service_json_p, "4 >>\n");
							#endif

						}		/* if (spw_run_flag) */

				}		/* if (service_name_p) */

		}


	#if SERVICE_PREFS_WIDGET_DEBUG >= DEBUG_FINE
	PrintJSON (stdout, service_json_p, "5 >>\n");
	#endif

	if (!success_flag)
		{
			json_object_clear (service_json_p);
			json_decref (service_json_p);
			service_json_p = NULL;
		}

	#if SERVICE_PREFS_WIDGET_DEBUG >= DEBUG_FINE
	PrintJSON (stdout, service_json_p, "6 >>\n");
	#endif


	return service_json_p;
}


