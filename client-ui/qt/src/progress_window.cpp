#include <QVBoxLayout>


#include "progress_widget.h"
#include "progress_window.h"
#include "json_tools.h"
#include "json_util.h"
#include "memory_allocations.h"
#include "qt_client_data.h"



ProgressWindow :: ProgressWindow (QMainWindow *parent_p, QTClientData *data_p)
: QWidget (parent_p),
	pw_data_p (data_p)
{
	QVBoxLayout *layout_p = new QVBoxLayout;

	setLayout (layout_p);

	setAcceptDrops (true);
}


ProgressWindow ::	~ProgressWindow ()
{
}


bool ProgressWindow :: AddProgressItemFromJSON (const json_t *json_p)
{
	bool success_flag = false;
	ProgressWidget *widget_p = ProgressWidget :: CreateProgressWidgetFromJSON (json_p);

	if (widget_p)
		{
			pw_widgets.append (widget_p);
			layout () -> addWidget (widget_p);

			success_flag = true;
		}

	return success_flag;
}



json_t *ProgressWindow :: BuildStatusRequest ()
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
					const char *username_s = NULL;
					const char *password_s = NULL;

					json_t *statuses_json_p = CallServices (req_p, username_s, password_s, pw_data_p -> qcd_base_data.cd_connection_p);

					if (statuses_json_p)
						{
							json_t *services_json_p = json_object_get (statuses_json_p, SERVICES_NAME_S);

							if (services_json_p)
								{
									if (json_is_array (services_json_p))
										{
											size_t num_services = json_array_size (services_json_p);
											ProgressWidget *progress_widget_p = 0;

											for (size_t i = 0; i < num_services; ++ i)
												{
													json_t *service_json_p = json_array_get (services_json_p, i);
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

																			while ((progress_widget_p == 0) && (j < num_services))
																				{
																					ProgressWidget *widget_p = pw_widgets.at (i);
																					const uuid_t *id_p = widget_p -> GetUUID ();

																					if (uuid_compare (*id_p, uuid))
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
																							ProgressWidget *widget_p = pw_widgets.at (i);
																							const uuid_t *id_p = widget_p -> GetUUID ();

																							if (uuid_compare (*id_p, uuid))
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
																					json_t *status_json_p = json_object_get (service_json_p, SERVICE_STATUS_S);
																				}		/* if (progress_widget_p) */

																		}

																}

														}		/* if (uuid_json_p) */



												}		/* for (size_t i = 0; i < num_services; ++ i) */

										}		/* if (json_is_array (services_json_p)) */

									for (size_t i = 0; i < size; ++ i)
										{
											* (ids_pp + i) = pw_widgets.at (i) -> GetUUID ();
										}

								}		/* if (services_json_p) */

						}		/* if (statuses_json_p) */

				}		/* if (req_p) */

			FreeMemory (ids_pp);
		}		/* if (ids_pp) */

	return req_p;
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




void ProgressWindow :: dropEvent (QDropEvent *event_p)
{

}


void ProgressWindow :: dragEnterEvent (QDragEnterEvent *event_p)
{

}
