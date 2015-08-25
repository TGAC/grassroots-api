#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QApplication>
#include <QDialog>
#include <QStyleFactory>
#include <QDialogButtonBox>
#include <QPushButton>

#include "client_ui_api.h"

#include "prefs_widget.h"
#include "results_widget.h"
#include "main_window.h"

#include "memory_allocations.h"
#include "string_utils.h"

#include "results_window.h"

#include "qt_client_data.h"
#include "progress_window.h"
#include "json_util.h"


#ifdef _DEBUG
	#define CLIENT_UI_API_DEBUG (DEBUG_FINE)
#else
	#define CLIENT_UI_API_DEBUG (DEBUG_NONE)
#endif

static int s_dummy_argc = 1;

static QTClientData *AllocateQTClientData (void);
static void FreeQTClientData (QTClientData *qt_data_p);

static const char *GetQTClientName (ClientData *client_data_p);
static const char *GetQTClientDescription (ClientData *client_data_p);
static json_t *RunQTClient (ClientData *client_data_p);
static int AddServiceToQTClient (ClientData *client_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s,  ParameterSet *params_p);
static json_t *DisplayResultsInQTClient (ClientData *client_data_p, const json_t *response_p);


Client *GetClient (Connection *connection_p)
{
	Client *client_p = NULL;
	QTClientData *data_p = AllocateQTClientData ();

	if (data_p)
		{
			client_p = (Client *) AllocMemory (sizeof (Client));

			if (client_p)
				{
					InitialiseClient (client_p, GetQTClientName, GetQTClientDescription, RunQTClient, DisplayResultsInQTClient, AddServiceToQTClient, reinterpret_cast <ClientData *> (data_p), connection_p);
				}
			else
				{
					FreeMemory (data_p);
				}

		}

	return client_p;
}


void ReleaseClient (Client *client_p)
{
	QTClientData *qt_data_p = reinterpret_cast <QTClientData *> (client_p -> cl_data_p);

	FreeQTClientData (qt_data_p);
	FreeMemory (client_p);
}


static QTClientData *AllocateQTClientData (void)
{
	QTClientData *data_p = (QTClientData *) AllocMemory (sizeof (QTClientData));

	if (data_p)
		{
			/*
			 * Before Qt widgets can be created a valid QApplication
			 * must be created. This requires a valid argc, argv pair
			 * that remain in scope for the entire lifetime of the QApplication
			 * object, In addition, argc must be greater than zero and argv must
			 * contain at least one valid character string.
			 */
			data_p -> qcd_dummy_arg_s = CopyToNewString ("WheatIS Tool", 0, false);

			if (data_p -> qcd_dummy_arg_s)
				{
					data_p -> qcd_app_p = new QApplication (s_dummy_argc, & (data_p -> qcd_dummy_arg_s));
					/*
					 * Ubuntu 12.04 has some theme bugs with various styles giving messages such as
					 *
					 * (client:1574): Gtk-CRITICAL **: IA__gtk_widget_style_get: assertion
					 * `GTK_IS_WIDGET (widget)' failed
					 *
					 * The solution is to use a theme that isn't broken on Ubuntu such as Plastique.
					 */
					QStyle *style_p = QStyleFactory :: create ("fusion");
					data_p -> qcd_app_p -> setStyle (style_p);

					data_p -> qcd_window_p = new MainWindow (data_p);
					data_p -> qcd_window_p -> setWindowIcon (QIcon ("images/cog"));

					QObject :: connect (data_p -> qcd_window_p, &MainWindow :: Closed, data_p -> qcd_app_p, &QApplication :: quit);
					data_p -> qcd_results_p = new ResultsWindow (data_p -> qcd_window_p);

					data_p -> qcd_progress_p = new ProgressWindow (data_p -> qcd_window_p, data_p);

					data_p -> qcd_init_flag = false;
				}
			else
				{
					FreeMemory (data_p);
					data_p = NULL;
				}
		}

	return data_p;
}


static void FreeQTClientData (QTClientData *qt_data_p)
{
	delete (qt_data_p -> qcd_window_p);
	delete (qt_data_p -> qcd_app_p);
	FreeCopiedString (qt_data_p -> qcd_dummy_arg_s);

	FreeMemory (qt_data_p);
}


static const char *GetQTClientName (ClientData *client_data_p)
{
	return "Qt-based WheatIS client";
}


static const char *GetQTClientDescription (ClientData *client_data_p)
{
	return "A Qt-based WheatIS client user interface";
}


static json_t *RunQTClient (ClientData *client_data_p)
{
	QTClientData *qt_data_p = reinterpret_cast <QTClientData *> (client_data_p);
	json_t *res_p = NULL;

	qt_data_p -> qcd_window_p -> show ();

	if (! (qt_data_p -> qcd_init_flag))
		{
			qt_data_p -> qcd_init_flag = true;
			int res = qt_data_p -> qcd_app_p -> exec ();
		}

	return res_p;
}


static int AddServiceToQTClient (ClientData *client_data_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, ParameterSet *params_p)
{
	int res = 0;
	QTClientData *qt_data_p = reinterpret_cast <QTClientData *> (client_data_p);

	qt_data_p -> qcd_window_p -> CreateAndAddServicePage (service_name_s, service_description_s, service_info_uri_s, params_p);

	return res;
}



static json_t *DisplayResultsInQTClient (ClientData *client_data_p, const json_t *response_p)
{
	json_t *res_p = NULL;

	#if CLIENT_UI_API_DEBUG >= DL_FINE
	PrintJSONToLog (response_p, "response:\n", STM_LEVEL_FINE);
	#endif

	QTClientData *qt_data_p = reinterpret_cast <QTClientData *> (client_data_p);
	const char *service_name_s = 0;
	const char *service_description_s = 0;
	const char *service_uri_s = 0;
	uint32 res = qt_data_p -> qcd_results_p -> AddAllResultsPagesFromJSON (response_p, service_name_s, service_description_s, service_uri_s);

	qt_data_p -> qcd_results_p -> show ();

	if (! (qt_data_p -> qcd_init_flag))
		{
			qt_data_p -> qcd_init_flag = true;
			qt_data_p -> qcd_app_p -> exec ();
		}


	return res_p;
}



