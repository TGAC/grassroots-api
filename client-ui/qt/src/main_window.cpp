/*
** Copyright 2014-2015 The Genome Analysis Centre
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
#include <QAction>
#include <QDockWidget>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMimeData>
#include <QPushButton>
#include <QPluginLoader>

#include <QUrl>
#include <QTabWidget>
#include <QToolBar>
#include <QWidget>

#include "string_utils.h"
#include "request_tools.h"

#include "json_tools.h"
#include "json_util.h"

#include "main_window.h"
#include "results_window.h"
#include "filesystem_utils.h"

#include "qt_client_data.h"
#include "ui_utils.h"

#ifdef _DEBUG
	#define MAIN_WINDOW_DEBUG	(STM_LEVEL_FINE)
#else
	#define MAIN_WINDOW_DEBUG	(STM_LEVEL_NONE)
#endif


MainWindow :: MainWindow (QTClientData *data_p)
: mw_client_data_p (data_p)
{
	setAcceptDrops (true);


	QTabWidget *tabs_p = new QTabWidget;
	setCentralWidget (tabs_p);

	mw_prefs_widget_p = new PrefsWidget (this, PL_BASIC, false, data_p);
	connect (mw_prefs_widget_p, &PrefsWidget :: RunServices, this, &MainWindow :: RunServices);
	tabs_p -> addTab (mw_prefs_widget_p, QIcon ("images/list_wand"), "All Services");

	mw_keyword_widget_p = new KeywordWidget (this, PL_BASIC);
	connect (mw_keyword_widget_p, &KeywordWidget :: RunKeywordSearch, this, &MainWindow :: RunKeywordSearch);
	tabs_p -> addTab (mw_keyword_widget_p, QIcon ("images/list_search"), "Run by search");

	setWindowTitle ("WheatIS Tool");
	setWindowIcon (QIcon ("images/cog"));

	AddActions ();

	QSize screen_size = QDesktopWidget ().availableGeometry (this).size ();
	resize (screen_size * 0.5);
	move (screen_size.width () * 0.25, screen_size.height () * 0.25);
}


MainWindow :: ~MainWindow ()
{
}

void MainWindow :: Accept ()
{
	RunServices (true);
}


void MainWindow :: Reject ()
{
	RunServices (false);
}


void MainWindow :: ProcessResults (json_t *services_json_p)
{
	PrintJSONToLog (services_json_p, "\n\nDATA:\n", STM_LEVEL_FINE, __FILE__, __LINE__);

	if (json_is_array (services_json_p))
		{
			int status;
			ProgressWindow *progress_p = mw_client_data_p -> qcd_progress_p;
			bool show_progress_flag = false;
			ResultsWindow *results_p = mw_client_data_p -> qcd_results_p;
			bool show_results_flag = false;

			size_t i;
			json_t *job_p;

			json_array_foreach (services_json_p, i, job_p)
				{
					const char *service_name_s = GetJSONString (job_p, SERVICE_NAME_S);

					if (service_name_s)
						{
							bool run_service_flag = false;

							GetJSONBoolean (job_p, SERVICE_RUN_S, &run_service_flag);

							if (run_service_flag)
								{
									results_p -> AddInterestedService (job_p, service_name_s);

								}		/* if (run_service_flag) */
							else
								{
									const char *service_description_s = GetJSONString (job_p, OPERATION_DESCRIPTION_S);
									const char *service_uri_s =  GetJSONString (job_p, OPERATION_INFORMATION_URI_S);

									/* Get the job status */
									OperationStatus status = OS_ERROR;
									const char *value_s = GetJSONString (job_p, SERVICE_STATUS_S);

									if (value_s)
										{
											status = GetOperationStatusFromString (value_s);
										}
									else
										{
											int i;
											/* Get the job status */

											if (GetJSONInteger(job_p, SERVICE_STATUS_VALUE_S, &i))
												{
													if ((i > OS_LOWER_LIMIT) && (i < OS_UPPER_LIMIT))
														{
															status = (OperationStatus) i;
														}
												}
										}

									if (status != OS_ERROR)
										{
											json_t *errors_p = NULL;

											if ((status == OS_SUCCEEDED) || (status == OS_PARTIALLY_SUCCEEDED))
												{
													results_p -> AddResultsPageFromJSON (job_p, service_name_s, service_description_s, service_uri_s);
													show_results_flag = true;
												}
											else
												{
													progress_p -> AddProgressItemFromJSON (job_p, service_name_s, service_description_s, service_uri_s);
													show_progress_flag = true;
												}

											errors_p = json_object_get (job_p, "errors");

											if (errors_p)
												{
													if (service_name_s)
														{
															mw_prefs_widget_p -> SetServiceErrors (service_name_s, errors_p);
														}
												}

										}		/* if (status != OS_ERROR) */

								}		/* /* if (run_service_flag) else */

						}		/* if (service_name_s) */

				}		/* json_array_foreach (services_json_p, i, job_p) */

			if (show_progress_flag)
				{
					progress_p -> show ();
					progress_p -> raise ();
				}

			if (show_results_flag)
				{
					results_p -> show ();
					results_p -> raise ();
				}

		}		/* if (json_is_array (services_json_p)) */

}



void MainWindow :: RunServices (bool run_flag)
{
	if (run_flag)
		{
			json_t *client_params_p = GetUserValuesAsJSON (false);

			if (client_params_p)
				{
					const char *username_s = NULL;
					const char *password_s = NULL;

					setCursor (Qt :: BusyCursor);
					json_t *services_json_p = CallServices (client_params_p, username_s, password_s, mw_client_data_p -> qcd_base_data.cd_connection_p);
					setCursor (Qt :: ArrowCursor);

					if (services_json_p)
						{
							ProcessResults (services_json_p);
							json_decref (services_json_p);
						}		/* if (services_json_p) */

					json_decref (client_params_p);
				}		/* if (client_params_p) */
		}
	else
		{
			close ();
		}
}


void MainWindow :: RunKeywordSearch (QString keywords)
{
	const char *username_s = NULL;
	const char *password_s = NULL;
	QByteArray ba = keywords.toLocal8Bit ();
	const char *keywords_s = ba.constData ();
	json_t *query_p  = GetKeywordServicesRequest (username_s, password_s, keywords_s);

	if (query_p)
		{
			json_t *results_p = NULL;

			PrintJSONToLog (query_p, "\n\nquery:\n", STM_LEVEL_FINE, __FILE__, __LINE__);

			setCursor (Qt :: BusyCursor);
			results_p = MakeRemoteJsonCall (query_p, mw_client_data_p -> qcd_base_data.cd_connection_p);
			setCursor (Qt :: ArrowCursor);

			if (results_p)
				{
					ProcessResults (results_p);
					json_decref (results_p);
				}		/* if (results_p) */

			json_decref (query_p);
		}		/* if (query_p) */

}


bool MainWindow :: AddResults (const json_t *service_results_p)
{
	bool success_flag = false;
	json_t *jobs_p = json_object_get (service_results_p, SERVICE_JOBS_S);

	if (jobs_p)
		{
			const char *service_name_s = GetJSONString (service_results_p, SERVICE_NAME_S);
			const char *service_description_s = GetJSONString (service_results_p, SERVICES_DESCRIPTION_S);
			const char *service_uri_s = NULL;

			if (json_is_array (jobs_p))
				{
					size_t i;
					json_t *job_p;

					json_array_foreach (jobs_p, i, job_p)
						{
							if (mw_client_data_p -> qcd_results_p -> AddResultsPageFromJSON (job_p, service_name_s,  service_description_s, service_uri_s))
								{
									success_flag = true;
								}
						}
				}
			else
				{
					success_flag = mw_client_data_p -> qcd_results_p -> AddResultsPageFromJSON (jobs_p, service_name_s,  service_description_s, service_uri_s);
				}
		}

	return success_flag;
}



void MainWindow :: CreateAndAddServicePage (const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, const json_t *provider_p, ParameterSet *params_p)
{
	mw_prefs_widget_p -> CreateAndAddServicePage (service_name_s, service_description_s, service_info_uri_s, provider_p, params_p);
}


json_t *MainWindow :: GetUserValuesAsJSON (bool all_flag) const
{
	return mw_prefs_widget_p -> GetUserValuesAsJSON (all_flag);
}


void MainWindow :: LoadConfiguration ()
{
	bool success_flag = false;

	QString filename = QFileDialog :: getOpenFileName (this, tr ("Save Configuration"), "wheatis_config.json", tr ("JSON (*.json)"));

	if (! (filename.isNull () || filename.isEmpty ()))
		{
			LoadConfigurationFile (filename);
		}		/* if (! (filename.isNull () || filename.isEmpty ())) */

}


void MainWindow :: SetParams (json_t *config_p)
{

	if (json_is_array (config_p))
		{
			const size_t num_services = json_array_size (config_p);
			size_t i = 0;

			for (i = 0; i < num_services; ++ i)
				{
					json_t *service_json_p = json_array_get (config_p, i);

				}		/* for (i = 0; i < num_services; ++ i) */

		}		/* if (json_is_array (config_p) */
	else
		{

		}
}


void MainWindow :: LoadConfigurationFile (QString &filename_r)
{
	char c = GetFileSeparatorChar ();
	if (c != '/')
		{
			filename_r.replace('/', c);
		}

	QByteArray ba = filename_r.toLocal8Bit ();
	const char * const filename_s = ba.constData ();

	json_error_t error;
	json_t *config_p = json_load_file (filename_s, 0, &error);


	if (config_p)
		{
			mw_prefs_widget_p -> SetServiceParams (config_p);
		}		/* if (config_p) */


}


void MainWindow :: SaveConfiguration ()
{
	QString filename = QFileDialog :: getSaveFileName (this, tr ("Save Configuration"), "wheatis_config.json", tr ("JSON (*.json)"));

	if (! (filename.isNull () || filename.isEmpty ()))
		{
			QByteArray ba = filename.toLocal8Bit ();
			const char * const filename_s = ba.constData ();

			json_t *res_p = GetUserValuesAsJSON (true);

			if (res_p)
				{
					int res = json_dump_file (res_p, filename_s, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

					if (res != 0)
						{

						}

					WipeJSON (res_p);
				}

		}		/* if (! (filename.isNull () || filename.isEmpty ())) */

}



void MainWindow :: dropEvent (QDropEvent *event_p)
{
	QList <QUrl> urls = event_p -> mimeData () -> urls ();

	if (! (urls.isEmpty ()))
		{
			QString filename = urls.first ().toLocalFile ();

			if (! ((filename.isEmpty ()) || (filename.isNull ())))
				{
					LoadConfigurationFile (filename);
				}		/* if (! (filename.isEmpty ())) */

		}		/* if (! (urls.isEmpty ())) */

}


void MainWindow :: dragEnterEvent (QDragEnterEvent *event_p)
{
	event_p -> acceptProposedAction ();
}


void MainWindow :: closeEvent (QCloseEvent *event_p)
{
	event_p -> accept ();
	emit Closed ();
}



void MainWindow :: AddActions ()
{
	QMenuBar *menu_bar_p = menuBar ();
	QToolBar *toolbar_p = new QToolBar (tr ("Main"));
	toolbar_p -> setToolButtonStyle (Qt :: ToolButtonFollowStyle);

	// File Menu
	QMenu *menu_p = menu_bar_p -> addMenu (tr ("&File"));

	// Load configuration
	QAction *action_p = new QAction (QIcon ("images/open"), tr ("&Load Configuration..."), this);
	action_p -> setShortcuts (QKeySequence :: Open);
	action_p -> setStatusTip (tr ("Load a configuration file."));
	connect (action_p, &QAction :: triggered, this, &MainWindow :: LoadConfiguration);
	menu_p -> addAction (action_p);
	toolbar_p -> addAction (action_p);

	// Save configuration
	action_p = new QAction (QIcon ("images/save"), tr ("&Save Configuration..."), this);
	action_p -> setShortcuts (QKeySequence :: Save);
	action_p -> setStatusTip (tr ("Save configuration file."));
	connect (action_p, &QAction :: triggered, this, &MainWindow :: SaveConfiguration);
	menu_p -> addAction (action_p);
	toolbar_p -> addAction (action_p);

	toolbar_p -> addSeparator ();

	// Run
	action_p = new QAction (QIcon ("images/run"), tr ("Run"), this);
	toolbar_p -> addAction (action_p);
	connect (action_p, &QAction :: triggered, 	this, &MainWindow :: Accept);

	// Cancel
	action_p = new QAction (QIcon ("images/cancel"), tr ("Quit"), this);
	toolbar_p -> addAction (action_p);
	connect (action_p, &QAction :: triggered, 	this, &MainWindow :: Accept);

	// Tools Menu
	menu_p = menu_bar_p -> addMenu (tr ("&Tools"));

	QMenu *sub_menu_p = menu_p -> addMenu (tr ("Level"));
	QActionGroup *interface_levels_p = new QActionGroup (this);

	// Easy Level
	action_p = new QAction (tr ("Basic"), this);
	action_p -> setStatusTip (tr ("Basic"));
	action_p -> setCheckable (true);
	action_p -> setChecked (true);
	connect (action_p, &QAction :: triggered, this, &MainWindow :: SetBasicInterfaceLevel);
	sub_menu_p -> addAction (action_p);
	interface_levels_p -> addAction (action_p);

	// Intermediate Level
	action_p = new QAction (tr ("Intermediate"), this);
	action_p -> setStatusTip (tr ("Intermediate"));
	action_p -> setCheckable (true);
	connect (action_p, &QAction :: triggered, this, &MainWindow :: SetIntermediateInterfaceLevel);
	sub_menu_p -> addAction (action_p);
	interface_levels_p -> addAction (action_p);

	// Advanced Lavel
	action_p = new QAction (tr ("Advanced"), this);
	action_p -> setStatusTip (tr ("Advanced"));
	action_p -> setCheckable (true);
	connect (action_p, &QAction :: triggered, this, &MainWindow :: SetAdvancedInterfaceLevel);
	sub_menu_p -> addAction (action_p);
	interface_levels_p -> addAction (action_p);


	addToolBar (toolbar_p);
}


void MainWindow :: SetBasicInterfaceLevel ()
{
	mw_prefs_widget_p -> SetInterfaceLevel (PL_BASIC);
}


void MainWindow :: SetIntermediateInterfaceLevel ()
{
	mw_prefs_widget_p -> SetInterfaceLevel (PL_INTERMEDIATE);
}


void MainWindow :: SetAdvancedInterfaceLevel ()
{
	mw_prefs_widget_p -> SetInterfaceLevel (PL_ADVANCED);
}



