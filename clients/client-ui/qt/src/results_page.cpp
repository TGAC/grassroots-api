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

#include <QDesktopServices>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QString>
#include <QLabel>

#include "results_page.h"
#include "results_widget.h"

#include "json_util.h"


ResultsPage :: ResultsPage (ResultsWidget *parent_p)
	: QWidget (parent_p)
{
	rp_results_list_p = new ResultsList (this);
	SetUp (parent_p, 0, 0, 0, 0);
}


ResultsPage :: ResultsPage (const json_t *results_list_json_p, const char * const job_name_s, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s, ResultsWidget *parent_p)
	: QWidget (parent_p)
{
	rp_results_list_p = new ResultsList (this);

	if (rp_results_list_p -> SetListFromJSON (job_name_s, results_list_json_p))
		{
			SetUp (parent_p, job_name_s, service_name_s, service_description_s, service_uri_s);
		}
	else
		{
			throw std::bad_alloc ();
		}
}


void ResultsPage :: SetUp (ResultsWidget *parent_p, const char * const job_name_s, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s)
{
	QVBoxLayout *layout_p = new QVBoxLayout;
	setLayout (layout_p);

	QFormLayout *labels_layout_p = new QFormLayout;

	rp_job_name_s = job_name_s;

	if (job_name_s)
		{
			QLabel *label_p = new QLabel (job_name_s);
			labels_layout_p -> addRow ("Job:", label_p);
		}


	if (service_name_s)
		{
			QLabel *label_p = new QLabel (service_name_s);
			labels_layout_p -> addRow ("Service:", label_p);
		}


	if (service_description_s)
		{
			QLabel *label_p = new QLabel (service_description_s);
			labels_layout_p -> addRow ("Description:", label_p);
		}

	if (service_uri_s)
		{
			QString s ("For more information, go to <a href=\"");
			s.append (service_uri_s);
			s.append ("\">");
			s.append (service_uri_s);
			s.append ("</a>");


			QLabel *label_p = new QLabel (s);
			label_p -> setOpenExternalLinks (true);
			labels_layout_p -> addRow ("Further information:", label_p);
		}

	layout_p -> addLayout (labels_layout_p);

	layout_p -> addWidget (rp_results_list_p);

	connect (this, &ResultsPage :: ServiceRequested, parent_p, &ResultsWidget :: SelectService);
	connect (this, &ResultsPage :: RunServiceRequested, parent_p, &ResultsWidget :: RunService);
}




ResultsPage :: ~ResultsPage ()
{

}


void ResultsPage :: SelectService (const char *service_name_s, const json_t *params_json_p)
{
	emit ServiceRequested (service_name_s, params_json_p);
}


void ResultsPage :: RunService (json_t *request_p)
{
	if (request_p)
		{
			emit RunServiceRequested (request_p);
		}
}



void ResultsPage :: OpenWebLink (const char * const uri_s)
{
	QUrl url (uri_s);

	if (!QDesktopServices :: openUrl (url))
		{
			QWebEngineView *browser_p = new QWebEngineView;

			rp_browsers.append (browser_p);
			browser_p -> load (url);
			browser_p -> show ();
		}
}




ResultsList *ResultsPage :: GetResultsList () const
{
	return rp_results_list_p;
}
