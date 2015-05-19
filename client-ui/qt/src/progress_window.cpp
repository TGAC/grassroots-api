#include <QVBoxLayout>
#include <QPushButton>


#include "progress_widget.h"
#include "progress_window.h"
#include "json_tools.h"
#include "json_util.h"
#include "memory_allocations.h"
#include "qt_client_data.h"



ProgressWindow :: ProgressWindow (QMainWindow *parent_p, QTClientData *data_p)
:	pw_data_p (data_p)
{

	setWindowTitle (tr ("Jobs Progress"));
	setWindowIcon (QIcon ("images/progress"));

  //pw_timer_p = new QTimer (this);
 // connect (pw_timer_p, &QTimer :: timeout, this, &ProgressWindow :: UpdateStatuses);

  pw_timer_started_flag = false;

	QPushButton *check_button_p = new QPushButton (QIcon ("images/check"), tr ("Check Statuses"));
  connect (check_button_p, &QPushButton :: clicked, this, &ProgressWindow :: UpdateStatuses);

	QPushButton *results_button_p = new QPushButton (QIcon ("images/go"), tr ("View Results"));
	connect (results_button_p, &QPushButton :: clicked, this, &ProgressWindow :: ViewResults);

	QHBoxLayout *buttons_layout_p = new QHBoxLayout;
	buttons_layout_p -> addWidget (check_button_p);
	buttons_layout_p -> addWidget (results_button_p);


  QVBoxLayout *layout_p = new QVBoxLayout;

  setLayout (layout_p);
	layout_p -> addLayout (buttons_layout_p);
}


ProgressWindow ::	~ProgressWindow ()
{
}



void ProgressWindow :: show ()
{
	QWidget :: show ();
}


bool ProgressWindow :: AddProgressItemFromJSON (const json_t *json_p)
{
	bool success_flag = false;
	ProgressWidget *widget_p = ProgressWidget :: CreateProgressWidgetFromJSON (json_p, this);

	if (widget_p)
		{
			pw_widgets.append (widget_p);
			layout () -> addWidget (widget_p);

			success_flag = true;
		}

	if (pw_timer_started_flag == false)
		{
//			pw_timer_p -> start (5000);
			pw_timer_started_flag = true;
		}


	return success_flag;
}



void ProgressWindow :: UpdateStatuses ()
{
	json_t *req_p = 0;
	const size_t size = pw_widgets.size ();
	const uuid_t **ids_pp = (const uuid_t **) AllocMemoryArray (size, sizeof (const uuid_t *));

	if (ids_pp)
		{
			for (size_t i = 0; i < size; ++ i)
				{
					* (ids_pp + i) = pw_widgets.at (i) -> GetUUID ();
				}

			req_p = GetServicesStatusRequest (ids_pp, size, pw_data_p -> qcd_base_data.cd_connection_p);

			if (req_p)
				{
					json_t *statuses_json_p = MakeRemoteJsonCall (req_p, pw_data_p -> qcd_base_data.cd_connection_p);

					if (statuses_json_p)
						{
							json_t *services_json_p = json_object_get (statuses_json_p, SERVICES_NAME_S);

							if (services_json_p)
								{
									if (json_is_array (services_json_p))
										{
											const size_t num_services = json_array_size (services_json_p);
											size_t i;
											json_t *service_json_p;

											json_array_foreach (services_json_p, i, service_json_p)
												{
													json_t *uuid_json_p = json_object_get (service_json_p, SERVICE_UUID_S);

													if (uuid_json_p)
														{
															if (json_is_string (uuid_json_p))
																{
																	const char *uuid_s = json_string_value (uuid_json_p);
																	uuid_t uuid;

																	if (uuid_parse (uuid_s, uuid) == 0)
																		{
																			size_t j = i;
																			ProgressWidget *progress_widget_p = 0;

																			while ((progress_widget_p == 0) && (j < num_services))
																				{
																					ProgressWidget *widget_p = pw_widgets.at (j);
																					const uuid_t *id_p = widget_p -> GetUUID ();

																					if (uuid_compare (*id_p, uuid) == 0)
																						{
																							progress_widget_p = widget_p;
																						}
																					else
																						{
																							++ j;
																						}
																				}

																			if (!progress_widget_p)
																				{
																					j = 0;

																					while ((progress_widget_p == 0) && (j < i))
																						{
																							ProgressWidget *widget_p = pw_widgets.at (j);
																							const uuid_t *id_p = widget_p -> GetUUID ();

																							if (uuid_compare (*id_p, uuid) == 0)
																								{
																									progress_widget_p = widget_p;
																								}
																							else
																								{
																									++ j;
																								}
																						}
																				}

																			if (progress_widget_p)
																				{
																					OperationStatus status;

																					if (GetStatusFromJSON (service_json_p, &status))
																						{
																							progress_widget_p -> SetStatus (status);
																						}

																				}		/* if (progress_widget_p) */
																		}
																}

														}		/* if (uuid_json_p) */



												}		/* for (size_t i = 0; i < num_services; ++ i) */

										}		/* if (json_is_array (services_json_p)) */


								}		/* if (services_json_p) */

						}		/* if (statuses_json_p) */

				}		/* if (req_p) */

			FreeMemory (ids_pp);
		}		/* if (ids_pp) */

}


void ProgressWindow :: ViewResults ()
{
	json_t *req_p = 0;
	const size_t size = pw_widgets.size ();
	const uuid_t **ids_pp = (const uuid_t **) AllocMemoryArray (size, sizeof (const uuid_t *));

	if (ids_pp)
		{
			for (size_t i = 0; i < size; ++ i)
				{
					* (ids_pp + i) = pw_widgets.at (i) -> GetUUID ();
				}

			req_p = GetServicesResultsRequest (ids_pp, size, pw_data_p -> qcd_base_data.cd_connection_p);

			if (req_p)
				{
					json_t *results_json_p = MakeRemoteJsonCall (req_p, pw_data_p -> qcd_base_data.cd_connection_p);

					if (results_json_p)
						{
							json_t *services_json_p = json_object_get (results_json_p, SERVICES_NAME_S);

							if (services_json_p)
								{
									if (json_is_array (services_json_p))
										{
											const size_t num_services = json_array_size (services_json_p);
											size_t i;
											json_t *service_json_p;

											json_array_foreach (services_json_p, i, service_json_p)
												{
													OperationStatus status;

													if (GetStatusFromJSON (service_json_p, &status))
														{
															if ((status == OS_FINISHED) || (status == OS_SUCCEEDED))
																{

																}		/* if ((status == OS_FINISHED) || (status == OS_SUCCEEDED)) */

														}		/* if (GetStatusFromJSON (service_json_p, &status)) */


															json_t *uuid_json_p = json_object_get (service_json_p, SERVICE_UUID_S);

															if (uuid_json_p)
																{
																	if (json_is_string (uuid_json_p))
																		{
																			const char *uuid_s = json_string_value (uuid_json_p);
																			uuid_t uuid;

																			if (uuid_parse (uuid_s, uuid) == 0)
																				{
																					size_t j = i;
																					ProgressWidget *progress_widget_p = 0;

																					while ((progress_widget_p == 0) && (j < num_services))
																						{
																							ProgressWidget *widget_p = pw_widgets.at (j);
																							const uuid_t *id_p = widget_p -> GetUUID ();

																							if (uuid_compare (*id_p, uuid) == 0)
																								{
																									progress_widget_p = widget_p;
																								}
																							else
																								{
																									++ j;
																								}
																						}

																					if (!progress_widget_p)
																						{
																							j = 0;

																							while ((progress_widget_p == 0) && (j < i))
																								{
																									ProgressWidget *widget_p = pw_widgets.at (j);
																									const uuid_t *id_p = widget_p -> GetUUID ();

																									if (uuid_compare (*id_p, uuid) == 0)
																										{
																											progress_widget_p = widget_p;
																										}
																									else
																										{
																											++ j;
																										}
																								}
																						}

																					if (progress_widget_p)
																						{
																							OperationStatus status;

																							if (GetStatusFromJSON (service_json_p, &status))
																								{
																									progress_widget_p -> SetStatus (status);
																								}

																						}		/* if (progress_widget_p) */
																				}
																		}

																}		/* if (uuid_json_p) */
														}		/* if (status_json_p) */

												}		/* for (size_t i = 0; i < num_services; ++ i) */

										}		/* if (json_is_array (services_json_p)) */


								}		/* if (services_json_p) */

						}		/* if (statuses_json_p) */

				}		/* if (req_p) */

			FreeMemory (ids_pp);
		}		/* if (ids_pp) */

}



json_t *ProgressWindow :: BuildResultsRequest ()
{
	json_t *req_p = 0;
	const size_t size = pw_widgets.size ();
	const uuid_t **ids_pp = (const uuid_t **) AllocMemoryArray (size, sizeof (const uuid_t *));

	if (ids_pp)
		{
			for (size_t i = 0; i < size; ++ i)
				{
					* (ids_pp + i) = pw_widgets.at (i) -> GetUUID ();
				}

			req_p = GetServicesStatusRequest (ids_pp, size, pw_data_p -> qcd_base_data.cd_connection_p);

			FreeMemory (ids_pp);
		}		/* if (ids_pp) */

	return req_p;
}

