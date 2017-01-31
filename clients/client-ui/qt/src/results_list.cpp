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
#include <QDesktopServices>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QErrorMessage>
#include <QApplication>

#include "results_list.h"
#include "results_page.h"
#include "standard_list_widget_item.h"
#include "json_list_widget_item.h"
#include "matched_service_list_widget_item.h"
#include "text_viewer.h"
#include "viewer_widget.h"

#include "json_util.h"
#include "data_resource.h"
#include "string_utils.h"
#include "filesystem_utils.h"


ResultsList :: ResultsList (ResultsPage *parent_p)
	:	QWidget (parent_p),
		rl_parent_p (parent_p)
{
	QLayout *layout_p = new QVBoxLayout;

	rl_list_p = new QListWidget;

	connect (rl_list_p, &QListWidget :: itemDoubleClicked, this,  &ResultsList :: OpenItemLink);

	layout_p -> addWidget (rl_list_p);

	setLayout (layout_p);
}


ResultsList :: ~ResultsList ()
{
}


void ResultsList :: OpenItemLink (QListWidgetItem *item_p)
{
	StandardListWidgetItem *our_item_p = dynamic_cast <StandardListWidgetItem *> (item_p);

	if (our_item_p)
		{
			setCursor (Qt :: BusyCursor);
			our_item_p -> ShowData ();
			setCursor (Qt :: ArrowCursor);
		}
	else
		{

		}
}


bool ResultsList :: SetListFromJSON (const char * const name_s, const json_t *results_list_json_p)
{
	bool success_flag = false;

	if (json_is_array (results_list_json_p))
		{
			const size_t size = json_array_size (results_list_json_p);

			if (size > 0)
				{
					size_t count = 0;

					rl_list_p -> clear ();

					for (size_t i = 0; i < size; ++ i)
						{
							json_t *value_p = json_array_get (results_list_json_p, i);

							if (AddItemFromJSON (name_s, value_p))
								{
									++ count;
								}

						}		/* for (size_t i = 0; i < size; ++ i) */

					success_flag = (count == size);
				}

		}		/* if (json_is_array (results_list_json_p)) */

	return success_flag;
}


bool ResultsList :: AddItemFromJSON (const char * const name_s, const json_t *resource_json_p)
{
	bool success_flag = false;
	const char *protocol_s = GetJSONString (resource_json_p, RESOURCE_PROTOCOL_S);

	if (protocol_s)
		{
			const char *title_s = GetJSONString (resource_json_p, RESOURCE_TITLE_S);
			const char *description_s = GetJSONString (resource_json_p, RESOURCE_DESCRIPTION_S);			
			const char *icon_path_s = NULL;

			if (strcmp (protocol_s, PROTOCOL_INLINE_S) == 0)
				{
					json_t *data_p = json_object_get (resource_json_p, RESOURCE_DATA_S);

					if (data_p)
						{
							QString s;

							if (name_s)
								{
									s.append (name_s);

									if (title_s)
										{
											s.append (" - ");
											s.append (title_s);
										}
								}
							else if (title_s)
								{
									s.append (title_s);
								}


							JSONListWidgetItem *item_p = new JSONListWidgetItem (s, rl_list_p);

							connect (item_p, &JSONListWidgetItem :: RunServiceRequested, rl_parent_p, &ResultsPage :: RunService);

							icon_path_s = "images/list_objects";

							item_p -> SetJSONData (data_p);

							if (!description_s)
								{
									description_s = "Click to view data";
								}

							item_p -> setToolTip (description_s);

							if (icon_path_s)
								{
									item_p -> setIcon (QIcon (icon_path_s));
								}

							success_flag = true;


							/* Are ther any linked services? */
							json_t *linked_services_p = json_object_get (data_p, LINKED_SERVICES_S);

							if (linked_services_p)
								{
									if (json_is_array (linked_services_p))
										{
											size_t i;
											json_t *linked_service_p;

											json_array_foreach (linked_services_p, i, linked_service_p)
												{
													QWidget *parent_p = rl_parent_p -> parentWidget ();
													ResultsWidget *rw_p = dynamic_cast <ResultsWidget *> (parent_p);

													bool b = rw_p-> AddResultsPageFromJSON (linked_service_p, "service_name_s", "service_description_s", "service_uri_s");

												}

										}

								}		/* if (linked_services_p) */

						}
				}
			else
				{
					const char *value_s = GetJSONString (resource_json_p, RESOURCE_VALUE_S);

					if (value_s)
						{
							StandardListWidgetItem *item_p = 0;

							if (strcmp (protocol_s, PROTOCOL_FILE_S) == 0)
								{
									icon_path_s = "images/list_file";
								}
							else	if ((strcmp (protocol_s, PROTOCOL_HTTP_S) == 0) || (strcmp (protocol_s, PROTOCOL_HTTPS_S) == 0))
								{
									icon_path_s = "images/list_internet";
								}
							else if (strcmp (protocol_s, PROTOCOL_IRODS_S) == 0)
								{
									icon_path_s = "images/list_filelink";
								}

							const char *item_name_s = name_s;

							if (!item_name_s)
								{
									item_name_s = title_s ? title_s : value_s;
								}

							item_p = new StandardListWidgetItem (item_name_s, rl_list_p);

							QString s (protocol_s);
							s.append ("://");
							s.append (value_s);
							QVariant v (s);

							item_p -> setToolTip (description_s ? description_s : value_s);
							item_p -> setData (Qt :: UserRole, v);


							connect (item_p,  &StandardListWidgetItem :: WebLinkSelected, rl_parent_p, &ResultsPage :: OpenWebLink);

							if (icon_path_s)
								{
									item_p -> setIcon (QIcon (icon_path_s));
								}

							success_flag = true;
						}
				}

		}		/* if (protocol_s) */
	else
		{
			bool service_flag = false;

			if (GetJSONBoolean (resource_json_p, SERVICE_RUN_S, &service_flag))
				{
					if (service_flag)
						{
							const char *title_s = GetJSONString (resource_json_p, SERVICE_NAME_S);
							const char *description_s = GetJSONString (resource_json_p, OPERATION_DESCRIPTION_S);
							json_t *data_p = json_object_get (resource_json_p, PARAM_SET_KEY_S);

							if (data_p)
								{
									ServiceListWidgetItem *item_p = new ServiceListWidgetItem (title_s, rl_list_p);
									const char *service_s = GetJSONString (resource_json_p, SERVICE_NAME_S);
									bool icon_flag = false;

									if (service_s)
										{
											char *icon_path_s = MakeFilename ("images", service_s);

											if (icon_path_s)
												{
													item_p -> setIcon (QIcon (icon_path_s));
													icon_flag = true;
													FreeCopiedString (icon_path_s);
												}
										}

									if (!icon_flag)
										{
											item_p -> setIcon (QIcon ("images/list_tool"));
										}

									item_p -> SetJSONData (resource_json_p);

									if (!description_s)
										{
											description_s = "Click to open service preferences";
										}

									item_p -> setToolTip (description_s);

									connect (item_p, &ServiceListWidgetItem :: ServiceRequested, rl_parent_p, &ResultsPage :: SelectService);

									success_flag = true;
								}

						}
				}

		}
	return success_flag;
}

