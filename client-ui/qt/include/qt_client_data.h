#ifndef QT_CLIENT_DATA_H
#define QT_CLIENT_DATA_H

#include <QApplication>

#include "client.h"

#include "main_window.h"
#include "results_window.h"
#include "progress_window.h"


typedef struct QTClientData
{
	ClientData qcd_base_data;
	QApplication *qcd_app_p;
	MainWindow *qcd_window_p;
	ResultsWindow *qcd_results_p;
	ProgressWindow *qcd_progress_p;
	char *qcd_dummy_arg_s;
	bool qcd_init_flag;
} QTClientData;


#endif // QT_CLIENT_DATA_H

