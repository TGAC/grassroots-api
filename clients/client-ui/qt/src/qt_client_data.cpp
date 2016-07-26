#include "qt_client_data.h"



ResultsWindow *GetNewResultsWindow (QTClientData *client_data_p)
{
	ResultsWindow *results_p = new ResultsWindow (client_data_p -> qcd_window_p, client_data_p);
	client_data_p -> qcd_results_widgets_p -> append (results_p);

	return results_p;
}



void RemoveResultsWindowFromQTClientData (QTClientData *client_data_p, ResultsWindow *results_p)
{
	client_data_p -> qcd_results_widgets_p -> removeOne (results_p);
}
