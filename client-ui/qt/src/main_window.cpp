#include <QAction>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMimeData>
#include <QPluginLoader>
#include <QUrl>
#include <QWidget>

#include "string_utils.h"
#include "request_tools.h"

#include "json_tools.h"

#include "main_window.h"
#include "filesystem_utils.h"

#include "qt_client_data.h"


MainWindow :: MainWindow (QTClientData *data_p)
: mw_client_data_p (data_p)
{
	setAcceptDrops (true);

	GenerateMenu ();

	mw_prefs_widget_p = new PrefsWidget (this, PL_BASIC, false);

	setCentralWidget (mw_prefs_widget_p);
	connect (mw_prefs_widget_p, &PrefsWidget :: RunServices, this, &MainWindow :: RunServices);

	setWindowTitle ("WheatIS Tool");
	setWindowIcon (QIcon ("images/cog"));

	QSize screen_size = QDesktopWidget ().availableGeometry (this).size ();
	resize (screen_size * 0.5);
	move (screen_size.width () * 0.25, screen_size.height () * 0.25);
}


MainWindow :: ~MainWindow ()
{
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
					uint32 i = mw_client_data_p -> qcd_results_p ->  AddAllResultsPagesFromJSON (services_json_p);
					mw_client_data_p -> qcd_results_p -> show ();
				}
		}
	else
		{
			close ();
		}
}

void MainWindow :: CreateAndAddServicePage (const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, ParameterSet *params_p)
{
	mw_prefs_widget_p -> CreateAndAddServicePage (service_name_s, service_description_s, service_info_uri_s, params_p);
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


void MainWindow :: LoadConfigurationFile (QString &filename_r)
{
	char c = GetFileSeparatorChar ();
	if (c != '/')
		{
			filename_r.replace('/', c);
		}

	QByteArray ba = filename_r.toLocal8Bit ();
	const char * const filename_s = ba.constData ();

	qDebug () << filename_r;
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

					json_object_clear (res_p);
					json_decref (res_p);
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



void MainWindow :: GenerateMenu ()
{
	QMenuBar *menu_bar_p = menuBar ();

	// File Menu
	QMenu *menu_p = menu_bar_p -> addMenu (tr ("&File"));

	// Load configuration
	QAction *action_p = new QAction (QIcon ("images/open"), tr ("&Load Configuration..."), this);
	action_p -> setShortcuts (QKeySequence :: Open);
	action_p -> setStatusTip (tr ("Load a configuration file."));
	connect (action_p, &QAction :: triggered, this, &MainWindow :: LoadConfiguration);
	menu_p -> addAction (action_p);

	// Save configuration
	action_p = new QAction (QIcon ("images/save"), tr ("&Save Configuration..."), this);
	action_p -> setShortcuts (QKeySequence :: Save);
	action_p -> setStatusTip (tr ("Save configuration file."));
	connect (action_p, &QAction :: triggered, this, &MainWindow :: SaveConfiguration);
	menu_p -> addAction (action_p);

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



