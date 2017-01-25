/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#include <QHBoxLayout>
#include <QLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include <QIcon>

#include "qt_client_data.h"
#include "service_prefs_widget.h"
#include "json_tools.h"
#include "service.h"
#include "streams.h"

#ifdef _DEBUG
	#define SERVICE_PREFS_WIDGET_DEBUG (DEBUG_FINE)
#else
	#define SERVICE_PREFS_WIDGET_DEBUG (DEBUG_NONE)
#endif

ServicePrefsWidget::ServicePrefsWidget (const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, const char * const service_icon_uri_s, const json_t *provider_p, ParameterSet *params_p, QTClientData *client_data_p, QWidget *parent_p)
: QWidget (parent_p),
	spw_service_name_s (service_name_s),
	spw_client_data_p (client_data_p)
{
	QLayout *layout_p = new QVBoxLayout;
	QScrollArea *scroller_p = new QScrollArea;
	scroller_p -> setBackgroundRole (QPalette :: Button);
	scroller_p -> setSizePolicy (QSizePolicy :: Expanding, QSizePolicy :: Expanding);
	spw_params_widget_p = new QTParameterWidget (service_name_s, service_description_s, service_info_uri_s, provider_p, params_p, NULL, PL_BASIC, spw_client_data_p);

	spw_params_widget_p -> setSizePolicy (QSizePolicy :: Expanding, QSizePolicy :: Expanding);
	scroller_p -> setWidget (spw_params_widget_p);
	scroller_p -> setWidgetResizable (true);
	layout_p -> addWidget (scroller_p);
	//layout_p -> addWidget (spw_params_widget_p);

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
	delete spw_params_widget_p;
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
	json_t *res_p = 0;
	ParameterSet *params_p = spw_params_widget_p -> GetParameterSet ();

	if (params_p)
		{
			const SchemaVersion *sv_p = spw_client_data_p -> qcd_base_data.cd_schema_p;

			const bool run_flag = GetRunFlag ();
			res_p = GetServiceRunRequest (spw_service_name_s, params_p, sv_p, run_flag);
		}

	return res_p;
}



bool ServicePrefsWidget :: SetServiceParams (const json_t *service_config_p)
{
	bool success_flag = true;
	const json_t *json_p = json_object_get (service_config_p, SERVICE_RUN_S);

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
			const json_t *params_json_p = json_object_get (json_p, PARAM_SET_PARAMS_S);

			if (params_json_p)
				{
					if (json_is_array (params_json_p))
						{
							json_t *param_p;
							size_t i;

							json_array_foreach (params_json_p, i, param_p)
								{
									const char *param_name_s = GetJSONString (param_p, PARAM_NAME_S);

									if (param_name_s)
										{
											BaseParamWidget *widget_p = spw_params_widget_p -> GetWidgetForParameter (param_name_s);

											if (widget_p)
												{
													const json_t *param_value_p = json_object_get (param_p, PARAM_CURRENT_VALUE_S);

													if (param_value_p)
														{
															if (! (widget_p -> SetValueFromJSON (param_value_p)))
																{
																	PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, param_value_p, "Failed to set %s -> %s from json", spw_service_name_s, param_name_s);
																}
														}
													else
														{
															PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, param_p, "Failed to get parameter value %s -> %s from json", spw_service_name_s, PARAM_CURRENT_VALUE_S);
														}

												}		/* if (widget_p) */
											else
												{
													PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, param_p, "Failed to get widget for %s -> %s from json", spw_service_name_s, param_name_s);
												}

										}		/* if (param_name_s) */
									else
										{
											PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, param_p, "Failed to get parameter name %s -> %s from json", spw_service_name_s, PARAM_NAME_S);
										}

								}		/* json_array_foreach (params_json_p, i, param_p) */

						}		/* if (json_is_array (params_json_p)) */

				}
			else
				{
					PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, json_p, "Failed to get child %s from json", PARAM_SET_PARAMS_S);
				}
		}
	else
		{
			PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, json_p, "Failed to get child %s from json", PARAM_SET_KEY_S);
		}

	return success_flag;
}


bool ServicePrefsWidget :: SetServiceErrors (const json_t * const errors_p)
{
	bool success_flag = false;

	if (json_is_array (errors_p))
		{
			json_t *error_p;
			size_t i;

			json_array_foreach (errors_p, i, error_p)
				{
					const char *param_name_s = GetJSONString (error_p, PARAM_NAME_S);

					if (param_name_s)
						{
							BaseParamWidget *widget_p = spw_params_widget_p -> GetWidgetForParameter (param_name_s);

							if (widget_p)
								{
									json_t *param_errors_p = json_object_get (error_p, PARAM_ERRORS_S);

									if (param_errors_p)
										{
											widget_p -> ShowErrors (param_errors_p);
										}

								}
						}		/* if (param_name_s) */


				}		/* json_array_foreach (json_p, i, param_p) */


		}		/* if (json_is_array (json_p)) */


	return success_flag;
}



void ServicePrefsWidget :: CheckInterfaceLevel (ParameterLevel level)
{
	spw_params_widget_p -> UpdateParameterLevel (level, this);
}


