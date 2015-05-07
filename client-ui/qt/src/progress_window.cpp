#include <QVBoxLayout>


#include "progress_widget.h"
#include "progress_window.h"
#include "json_tools.h"


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

					json_t *services_json_p = CallServices (client_params_p, username_s, password_s, mw_client_data_p -> qcd_base_data.cd_connection_p);

					if (services_json_p)
						{
							uint32 i = mw_client_data_p -> qcd_results_p ->  AddAllResultsPagesFromJSON (services_json_p);

							UIUtils :: CentreWidget (this, mw_client_data_p -> qcd_results_p);

							mw_client_data_p -> qcd_results_p -> show ();
						}
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
