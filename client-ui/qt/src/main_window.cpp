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


void MainWindow :: RunServices (bool run_flag)
{
	if (run_flag)
		{
			json_t *client_params_p = GetUserValuesAsJSON (false);
			const char *username_s = NULL;
			const char *password_s = NULL;

			json_t *services_json_p = CallServices (client_params_p, username_s, password_s, mw_client_data_p -> qcd_base_data.cd_connection_p);

			if (services_json_p)
				{

					PrintJSONToLog (services_json_p, "\n\nDATA:\n", STM_LEVEL_FINE);

					if (json_is_array (services_json_p))
						{
							int status;
							ProgressWindow *progress_p = mw_client_data_p -> qcd_progress_p;
							bool show_progress_flag = false;
							ResultsWindow *results_p = mw_client_data_p -> qcd_results_p;
							bool show_results_flag = false;

							size_t i;
							json_t *service_json_p;

							json_array_foreach (services_json_p, i, service_json_p)
								{
									const char *service_name_s = NULL;
									const char *service_description_s = NULL;
									const char *service_uri_s = NULL;
									json_t *metadata_p = json_object_get (service_json_p, SERVICE_METADATA_S);
									json_t *jobs_array_p = json_object_get (service_json_p, SERVICE_JOBS_S);

									if (metadata_p)
										{
											service_name_s = GetJSONString (metadata_p, SERVICE_NAME_S);
											service_description_s = GetJSONString (metadata_p, OPERATION_DESCRIPTION_S);
											service_uri_s = GetJSONString (metadata_p, OPERATION_INFORMATION_URI_S);
										}

									if (jobs_array_p && json_is_array (jobs_array_p))
										{
											size_t j;
											json_t *job_p;

											json_array_foreach (jobs_array_p, j, job_p)
												{
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
																	results_p -> AddAllResultsPagesFromJSON (job_p, service_name_s, service_description_s, service_uri_s);
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
														}


                        }   /* json_array_foreach (jobs_array_p, j, job_p) */

										}		/* if (jobs_array_p) */
								}

							if (show_progress_flag)
								{
									progress_p -> show ();
								}

							if (show_results_flag)
								{
									results_p -> show ();
								}
						}


					/*
					uint32 i = mw_client_data_p -> qcd_results_p ->  AddAllResultsPagesFromJSON (services_json_p);

					UIUtils :: CentreWidget (this, mw_client_data_p -> qcd_results_p);

					mw_client_data_p -> qcd_results_p -> show ();
					*/
				}
		}
	else
		{
			close ();
		}
}


void MainWindow :: RunKeywordSearch (QString keywords)
{

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

	QMenu *sub_menu_p = new QMenu (tr ("Level"));
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


	menu_p -> addMenu (sub_menu_p);

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



