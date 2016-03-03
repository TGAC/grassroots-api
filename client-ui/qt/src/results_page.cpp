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

#include <QVBoxLayout>
#include <QString>
#include <QLabel>

#include "results_page.h"


ResultsPage :: ResultsPage (QWidget *parent_p)
	: QWidget (parent_p)
{
	rp_results_list_p = new ResultsList (this);

	QVBoxLayout *layout_p = new QVBoxLayout;
	setLayout (layout_p);

	rp_description_label_p = new QLabel ();
	layout_p -> addWidget (rp_description_label_p);

	rp_uri_label_p = new QLabel ();
	layout_p -> addWidget (rp_uri_label_p);

	layout_p -> addWidget (rp_results_list_p);
}


ResultsPage :: ResultsPage (const json_t *results_list_json_p, const char * const description_s, const char * const uri_s, QWidget *parent_p)
	: QWidget (parent_p)
{
	rp_results_list_p = new ResultsList (this);

	if (rp_results_list_p -> SetListFromJSON (results_list_json_p))
		{
			QVBoxLayout *layout_p = new QVBoxLayout;
			setLayout (layout_p);

			QLabel *label_p = new QLabel (QString (description_s), this);
			layout_p -> addWidget (label_p);

			if (uri_s)
				{
					QString s ("For more information, go to <a href=\"");
					s.append (uri_s);
					s.append ("\">");
					s.append (uri_s);
					s.append ("</a>");

					label_p = new QLabel (s, this);
					label_p -> setOpenExternalLinks (true);
					layout_p -> addWidget (label_p);
				}

			layout_p -> addWidget (rp_results_list_p);
		}
	else
		{
			throw std::bad_alloc ();
		}
}


ResultsList *ResultsPage :: GetResultsList () const
{
	return rp_results_list_p;
}
