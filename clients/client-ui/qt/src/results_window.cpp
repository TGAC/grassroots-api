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
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMimeData>
#include <QDebug>


#include "results_window.h"
#include "main_window.h"

#include "json_util.h"
#include "json_tools.h"

#include "streams.h"


ResultsWindow :: ResultsWindow (MainWindow *parent_p, QTClientData *client_data_p)
	: rw_data_p (0),
		rw_client_data_p (client_data_p)
{
	QVBoxLayout *layout_p = new QVBoxLayout;

	rw_results_p = new ResultsWidget (this);

	connect (rw_results_p, &ResultsWidget :: ServiceRequested, parent_p, &MainWindow :: SelectService);
	connect (rw_results_p, &ResultsWidget :: RunServiceRequested, parent_p, &MainWindow :: RunService);


	layout_p -> addWidget (rw_results_p);

	QGroupBox *buttons_p = new QGroupBox;
	QHBoxLayout *buttons_layout_p = new QHBoxLayout;

	QPushButton *btn_p = new QPushButton (QIcon ("images/ok"), tr ("Ok"), buttons_p);
	buttons_layout_p -> addWidget (btn_p);
	connect (btn_p, &QPushButton :: clicked, this, &ResultsWindow :: close);


	btn_p = new QPushButton (QIcon ("images/save"), tr ("Save"), buttons_p);
	buttons_layout_p -> addWidget (btn_p);
	connect (btn_p, &QPushButton :: clicked, this, &ResultsWindow :: SaveResults);

	btn_p = new QPushButton (QIcon ("images/save"), tr ("Save Compact"), buttons_p);
	buttons_layout_p -> addWidget (btn_p);
	connect (btn_p, &QPushButton :: clicked, this, &ResultsWindow :: SaveCompactResults);

	btn_p = new QPushButton (QIcon ("images/saveall"), tr ("Save All Results"), buttons_p);
	buttons_layout_p -> addWidget (btn_p);
	connect (btn_p, &QPushButton :: clicked, this, &ResultsWindow :: SaveAllResults);


	buttons_p -> setLayout (buttons_layout_p);

	layout_p -> addWidget (buttons_p);

	setWindowTitle (tr ("Results"));
	setWindowIcon (QIcon ("images/viewas_list"));
	setLayout (layout_p);

	setAcceptDrops (true);
}




void ResultsWindow :: ClearData ()
{
	rw_results_p -> ClearData();

	if (rw_data_p)
		{
			WipeJSON (rw_data_p);
		}
}


bool ResultsWindow :: AddData (json_t *data_p)
{
	bool success_flag = false;

	if (!rw_data_p)
		{
			rw_data_p = json_array ();
		}

	if (rw_data_p)
		{
			success_flag = (json_array_append (rw_data_p, data_p) == 0);
		}

	return success_flag;
}



ResultsWindow :: ~ResultsWindow ()
{
  ClearData ();

	RemoveResultsWindowFromQTClientData (rw_client_data_p, this);
}


uint32 ResultsWindow :: AddAllResultsPagesFromJSON (json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s)
{  
  ClearData ();
  rw_data_p = json_p;

  return rw_results_p -> AddAllResultsPagesFromJSON (json_p, service_name_s, service_description_s, service_uri_s);
}


bool ResultsWindow :: AddResultsPageFromJSON (json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s)
{
	AddData (json_p);

	return rw_results_p -> AddResultsPageFromJSON (json_p, service_name_s, service_description_s, service_uri_s);
}


bool ResultsWindow :: AddInterestedService (json_t *job_p, const char *service_name_s)
{
	return rw_results_p -> AddInterestedService (job_p, service_name_s);
}



void ResultsWindow :: SaveJSONResults (const json_t * const results_p)
{
	QString filename = QFileDialog :: getSaveFileName (this, tr ("Save Results"), "wheatis_results.json", tr ("JSON (*.json)"));

	if (! (filename.isNull () || filename.isEmpty ()))
		{
			QByteArray ba = filename.toLocal8Bit ();
			const char * const filename_s = ba.constData ();
			int res = json_dump_file (results_p, filename_s, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

			if (res != 0)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to save results to %s", filename_s);
				}

		}		/* if (! (filename.isNull () || filename.isEmpty ())) */

}


void ResultsWindow :: SaveAllResults (bool clicked_flag)
{
	SaveJSONResults (rw_data_p);
}


void ResultsWindow :: SaveResults (bool clicked_flag)
{
	SaveJSONResults (rw_data_p);
}


bool ResultsWindow :: AddJSONDataToArray (json_t *results_p, const json_t *doc_p)
{
	json_t *data_p = json_object_get (doc_p, RESOURCE_DATA_S);

	if (data_p)
		{
			if (json_array_append (results_p, data_p) == 0)
				{
					return true;
				}
		}

	return false;
}


void ResultsWindow :: SaveCompactResults (bool clicked_flag)
{
	json_t *compact_results_p = json_array ();

	if (compact_results_p)
		{
			bool success_flag = true;

			char *dump_s = json_dumps (rw_data_p, JSON_INDENT (2));

			if (json_is_array (rw_data_p))
				{
					size_t i;
					const size_t size = json_array_size (rw_data_p);

					for (i = 0; i < size; ++ i)
						{
							json_t *src_p = json_array_get (rw_data_p, i);

							if (!AddJSONDataToArray (compact_results_p, src_p))
								{
									success_flag = false;
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add item to results for saving");
									i = size;
								}
						}
				}
			else
				{
					if (!AddJSONDataToArray (compact_results_p, rw_data_p))
						{
							success_flag = false;
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add item to results for saving");
						}
				}

			if (success_flag)
				{
					SaveJSONResults (compact_results_p);
				}

			json_decref (compact_results_p);
		}
}



void ResultsWindow :: LoadResults (const char * const filename_s)
{
	json_error_t error;

	json_t *results_p =	json_load_file (filename_s, 0, &error);

	if (results_p)
		{
			const char *name_s = NULL;
			const char *description_s = NULL;
			const char *uri_s = NULL;

			json_t *metadata_p = json_object_get (results_p, SERVICE_METADATA_S);

			if (metadata_p)
				{
					name_s = GetJSONString (metadata_p, SERVICE_NAME_S);
					description_s = GetJSONString (metadata_p, OPERATION_DESCRIPTION_S);
					uri_s = GetJSONString (metadata_p, OPERATION_INFORMATION_URI_S);
				}

			rw_results_p -> AddAllResultsPagesFromJSON (results_p, name_s, description_s, uri_s);
		}
}


void ResultsWindow :: LoadResults ()
{
	QString filename = QFileDialog :: getSaveFileName (this, tr ("Load Results"), "wheatis_results.json", tr ("JSON (*.json)"));

	if (! (filename.isNull () || filename.isEmpty ()))
		{
			QByteArray ba = filename.toLocal8Bit ();
			const char * const filename_s = ba.constData ();

			LoadResults (filename_s);
		}		/* if (! (filename.isNull () || filename.isEmpty ())) */

}


void ResultsWindow :: dropEvent (QDropEvent *event_p)
{
	QList <QUrl> urls = event_p -> mimeData () -> urls ();

	if (! (urls.isEmpty ()))
		{
			QString filename = urls.first ().toLocalFile ();

			if (! ((filename.isEmpty ()) || (filename.isNull ())))
				{
					QByteArray ba = filename.toLocal8Bit ();
					const char * const filename_s = ba.constData ();

					qDebug () << "dropped " << filename;

					LoadResults (filename_s);
				}		/* if (! (filename.isEmpty ())) */

		}		/* if (! (urls.isEmpty ())) */

}


void ResultsWindow :: dragEnterEvent (QDragEnterEvent *event_p)
{
	event_p -> acceptProposedAction ();
}

