#include <QHBoxLayout>
#include <QLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include <QIcon>

#include "service_prefs_widget.h"
#include "json_tools.h"


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
	QScrollArea *scroller_p = new QScrollArea;
	scroller_p -> setBackgroundRole (QPalette :: Button);

	spw_params_widget_p = new QTParameterWidget (service_name_s, service_description_s, service_info_uri_s, params_p, NULL, PL_BASIC);
	scroller_p -> setWidget (spw_params_widget_p);

	layout_p -> addWidget (scroller_p);

	QString s ("Run ");
	s.append (service_name_s);

	spw_run_service_button_p = new QCheckBox (s, this);
	spw_run_service_button_p -> setChecked (Qt :: Unchecked);
	connect (spw_run_service_button_p, &QCheckBox :: stateChanged, this, &ServicePrefsWidget :: SetRunFlag);

	QPushButton *reset_button_p = new QPushButton (QIcon ("images/reload"), "Restore Defaults", this);
	connect (reset_button_p, &QAbstractButton :: clicked, spw_params_widget_p, &QTParameterWidget :: ResetToDefaults);

	QVBoxLayout *buttons_layout_p = new QVBoxLayout;
	buttons_layout_p -> addWidget (spw_run_service_button_p);
	buttons_layout_p -> addWidget (reset_button_p);
	buttons_layout_p -> setAlignment (reset_button_p, Qt::AlignRight);


	layout_p -> addItem (buttons_layout_p);

	setLayout (layout_p);
}


ServicePrefsWidget :: ~ServicePrefsWidget ()
{

}


const char *ServicePrefsWidget ::GetServiceName () const
{
	return spw_service_name_s;
}


void ServicePrefsWidget :: ToggleRunFlag ()
{
	if (GetRunFlag ())
		{
			spw_run_service_button_p -> setCheckState (Qt :: Unchecked);
		}
	else
		{
			spw_run_service_button_p -> setCheckState (Qt :: Checked);
		}
}


void ServicePrefsWidget :: SetRunFlag (bool state)
{
	spw_run_service_button_p -> setCheckState (state ? Qt :: Checked : Qt :: Unchecked);
	emit RunStatusChanged (spw_service_name_s ,state);
}


bool ServicePrefsWidget :: GetRunFlag () const
{
	return (spw_run_service_button_p -> checkState() == Qt :: Checked);
}


json_t *ServicePrefsWidget :: GetServiceParamsAsJSON (bool full_flag) const
{
	bool success_flag = true;
	json_t *service_json_p = json_object ();

	if (service_json_p)
		{
			const bool run_flag = GetRunFlag ();

			success_flag = (json_object_set_new (service_json_p, SERVICES_NAME_S, json_string (spw_service_name_s)) == 0) && (json_object_set_new (service_json_p, SERVICE_RUN_S, run_flag ? json_true () : json_false ()) == 0);

			if (success_flag)
				{
					if (run_flag || full_flag)
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
											WipeJSON (param_set_json_p);
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
			WipeJSON (service_json_p);
			service_json_p = NULL;
		}

	#if SERVICE_PREFS_WIDGET_DEBUG >= DEBUG_FINE
	PrintJSON (stdout, service_json_p, "6 >>\n");
	#endif


	return service_json_p;
}



bool ServicePrefsWidget :: SetServiceParams (json_t *service_config_p)
{
	bool success_flag = true;
	json_t *json_p = json_object_get (service_config_p, SERVICE_RUN_S);

	if (json_p && (json_is_true (json_p)))
		{
			SetRunFlag (true);
		}
	else
		{
			SetRunFlag (false);
		}


	/* Set the params */
	json_p = json_object_get (service_config_p, PARAM_SET_KEY_S);

	if (json_p)
		{
			json_p = json_object_get (json_p, PARAM_SET_PARAMS_S);

			if (json_p)
				{
					if (json_is_array (json_p))
						{
							json_t *param_p;
							size_t i;

							json_array_foreach (json_p, i, param_p)
								{
									const char *param_name_s = GetJSONString (param_p, PARAM_NAME_S);
									const char *param_value_s = GetJSONString (param_p, PARAM_CURRENT_VALUE_S);

									if (param_name_s)
										{
											BaseParamWidget *widget_p = spw_params_widget_p -> GetWidgetForParameter (param_name_s);

											if (widget_p)
												{
													if (param_value_s)
														{
															if (! (widget_p -> SetValueFromText (param_value_s)))
																{

																}
														}

												}
										}		/* if (param_name_s) */


								}		/* json_array_foreach (json_p, i, param_p) */


						}		/* if (json_is_array (json_p)) */

				}

		}


	return success_flag;
}


void ServicePrefsWidget :: CheckInterfaceLevel (ParameterLevel level)
{
	spw_params_widget_p -> UpdateParameterLevel (level, this);
}


