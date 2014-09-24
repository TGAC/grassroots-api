#include <QHBoxLayout>
#include <QWidget>
#include <QApplication>
#include <QDialog>


#include "client_ui_api.h"

#include "prefs_widget.h"

#include "memory_allocations.h"

typedef struct QTClientData
{
	ClientData qcd_base_data;
	QApplication *qcd_app_p;
	QDialog *qcd_window_p;
	PrefsWidget *qcd_prefs_widget_p;
}QTClientData;

static int s_dummy_argc = 1;
static char *s_dummy_arg_s = "WheatIS";

static QTClientData *AllocateQTClientData (void);
static void FreeQTClientData (QTClientData *qt_data_p);

static const char *GetQTClientName (void);
static const char *GetQTClientDescription (void);
static int RunQTClient (ClientData *client_data_p);
static int AddServiceToQTClient (ClientData *client_p, const char * const service_name_s, const char * const service_description_s, ParameterSet *params_p);



Client *GetClient (void)
{
	Client *client_p = NULL;
	QTClientData *data_p = AllocateQTClientData ();

	if (data_p)
		{
			client_p = (Client *) AllocMemory (sizeof (Client));

			if (client_p)
				{
					InitialiseClient (client_p, GetQTClientName, GetQTClientDescription, RunQTClient, AddServiceToQTClient, reinterpret_cast <ClientData *> (data_p));
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
			 * Before Qt widgets can be created a valid QGUIApplication
			 * must be created. This requires a valid argc, argv pair
			 * that remain in scope for the entire lifetime of the QGUIApplication
			 * object, In addition, argc must be greater than zero and argv must
			 * contain at least one valid character string.
			 */

			data_p -> qcd_app_p = new QApplication (s_dummy_argc, &s_dummy_arg_s);

			QHBoxLayout *layout_p = new QHBoxLayout;

			data_p -> qcd_window_p = new QDialog;
			data_p -> qcd_window_p -> setLayout (layout_p);

			data_p -> qcd_prefs_widget_p = new PrefsWidget (data_p -> qcd_window_p, PL_BASIC);
			layout_p -> addWidget (data_p -> qcd_prefs_widget_p);
		}

	return data_p;
}


static void FreeQTClientData (QTClientData *qt_data_p)
{
	delete (qt_data_p -> qcd_window_p);
	delete (qt_data_p -> qcd_app_p);

	FreeMemory (qt_data_p);
}


static const char *GetQTClientName (void)
{
	return "Qt-based WheatIS client";
}


static const char *GetQTClientDescription (void)
{
	return "Qt-based WheatIS client";
}


static int RunQTClient (ClientData *client_data_p)
{
	QTClientData *qt_data_p = reinterpret_cast <QTClientData *> (client_data_p);
	int res = qt_data_p -> qcd_window_p -> exec ();

	return res;
}


static int AddServiceToQTClient (ClientData *client_data_p, const char * const service_name_s, const char * const service_description_s, ParameterSet *params_p)
{
	int res = -1;
	QTClientData *qt_data_p = reinterpret_cast <QTClientData *> (client_data_p);

	qt_data_p -> qcd_prefs_widget_p -> AddServicePage (service_name_s, service_description_s, params_p);


	return res;
}

