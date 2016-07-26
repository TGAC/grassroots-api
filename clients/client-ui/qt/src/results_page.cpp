/*
** Copyright 2014-2015 The Genome Analysis Centre
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
#include <QVBoxLayout>
#include <QString>
#include <QLabel>

#include "results_page.h"
#include "results_widget.h"



ResultsPage :: ResultsPage (ResultsWidget *parent_p)
	: QWidget (parent_p)
{
	rp_results_list_p = new ResultsList (this);
	SetUp (parent_p);
}



void ResultsPage :: SetUp (ResultsWidget *parent_p, const char * const description_s, const char * const uri_s)
{
	QVBoxLayout *layout_p = new QVBoxLayout;
	setLayout (layout_p);

	rp_description_label_p = new QLabel ();

	if (description_s)
		{
			rp_description_label_p -> setText (description_s);
		}

	layout_p -> addWidget (rp_description_label_p);

	rp_uri_label_p = new QLabel ();
	layout_p -> addWidget (rp_uri_label_p);


	if (uri_s)
		{
			QString s ("For more information, go to <a href=\"");
			s.append (uri_s);
			s.append ("\">");
			s.append (uri_s);
			s.append ("</a>");

			rp_uri_label_p -> setText (s);
			rp_uri_label_p -> setOpenExternalLinks (true);
		}

	layout_p -> addWidget (rp_results_list_p);

	connect (this, &ResultsPage :: ServiceRequested, parent_p, &ResultsWidget :: SelectService);
}


ResultsPage :: ResultsPage (const json_t *results_list_json_p, const char * const description_s, const char * const uri_s, ResultsWidget *parent_p)
	: QWidget (parent_p)
{
	rp_results_list_p = new ResultsList (this);

	if (rp_results_list_p -> SetListFromJSON (results_list_json_p))
		{
			SetUp (parent_p, description_s, uri_s);
		}
	else
		{
			throw std::bad_alloc ();
		}
}



ResultsPage :: ~ResultsPage ()
{

}


void ResultsPage :: SelectService (const char *service_name_s, const json_t *params_json_p)
{
	emit ServiceRequested (service_name_s, params_json_p);
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
