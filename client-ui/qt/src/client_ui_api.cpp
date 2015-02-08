#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QApplication>
#include <QDialog>
#include <QStyleFactory>


#include "client_ui_api.h"

#include "prefs_widget.h"
#include "results_widget.h"

#include "memory_allocations.h"
#include "string_utils.h"

typedef struct QTClientData
{
	ClientData qcd_base_data;
	QApplication *qcd_app_p;
	QDialog *qcd_window_p;
	PrefsWidget *qcd_prefs_widget_p;
	ResultsWidget *qcd_results_p;
	char *qcd_dummy_arg_s;
}QTClientData;

static int s_dummy_argc = 1;

static QTClientData *AllocateQTClientData (void);
static void FreeQTClientData (QTClientData *qt_data_p);

static const char *GetQTClientName (ClientData *client_data_p);
static const char *GetQTClientDescription (ClientData *client_data_p);
static json_t *RunQTClient (ClientData *client_data_p);
static int AddServiceToQTClient (ClientData *client_p, const char * const service_name_s, const char * const service_description_s, ParameterSet *params_p);
static json_t *DisplayResultsInQTClient (ClientData *client_data_p, const json_t *response_p);


Client *GetClient (void)
{
	Client *client_p = NULL;
	QTClientData *data_p = AllocateQTClientData ();

	if (data_p)
		{
			client_p = (Client *) AllocMemory (sizeof (Client));

			if (client_p)
				{
					InitialiseClient (client_p, GetQTClientName, GetQTClientDescription, RunQTClient, DisplayResultsInQTClient, AddServiceToQTClient, reinterpret_cast <ClientData *> (data_p));
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
			data_p -> qcd_dummy_arg_s = CopyToNewString ("WheatIS", 0, false);

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

					QHBoxLayout *layout_p = new QHBoxLayout;

					data_p -> qcd_window_p = new QDialog;
					data_p -> qcd_window_p -> setLayout (layout_p);

					data_p -> qcd_prefs_widget_p = new PrefsWidget (data_p -> qcd_window_p, PL_BASIC);
					layout_p -> addWidget (data_p -> qcd_prefs_widget_p);

					data_p -> qcd_results_p = new ResultsWidget;

					QObject :: connect (data_p -> qcd_prefs_widget_p, &PrefsWidget :: Finished, data_p -> qcd_window_p, &QDialog :: done);

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
	int res = qt_data_p -> qcd_window_p -> exec ();

	printf ("res %d\n", res);

	/* Did the user choose to run anything? */
	if (res == QDialog :: Accepted)
		{
			res_p = qt_data_p -> qcd_prefs_widget_p -> GetUserValuesAsJSON ();
		}


	return res_p;
}


static int AddServiceToQTClient (ClientData *client_data_p, const char * const service_name_s, const char * const service_description_s, ParameterSet *params_p)
{
	int res = 0;
	QTClientData *qt_data_p = reinterpret_cast <QTClientData *> (client_data_p);

	qt_data_p -> qcd_prefs_widget_p -> CreateAndAddServicePage (service_name_s, service_description_s, params_p);

	return res;
}



static json_t *DisplayResultsInQTClient (ClientData *client_data_p, const json_t *response_p)
{
	json_t *res_p = NULL;
	char *dump_s = json_dumps (response_p, JSON_INDENT (2));

	QTClientData *qt_data_p = reinterpret_cast <QTClientData *> (client_data_p);

	uint32 res = qt_data_p -> qcd_results_p -> AddAllResultsPagesFromJSON (response_p);
	//qt_data_p -> qcd_results_p -> show ();


	QDialog dialog;
	QVBoxLayout *layout_p = new QVBoxLayout;

	layout_p -> addWidget (qt_data_p -> qcd_results_p);
	dialog.setLayout (layout_p);

	dialog.exec ();

	free (dump_s);
	return res_p;
}
