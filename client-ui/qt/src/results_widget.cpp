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
#include <QLabel>

#include "results_widget.h"

#include "results_list.h"
#include "json_util.h"
#include "streams.h"


QString ResultsWidget :: RW_SERVICES_TAB_TITLE = QString ("Services");


ResultsWidget :: ResultsWidget (QWidget *parent_p)
 : QTabWidget (parent_p)
{
}

ResultsWidget :: ~ResultsWidget ()
{

}


void ResultsWidget :: ClearData ()
{
	/*
	 * Avoid any repaints until we've added all of the pages
	 */
	setUpdatesEnabled (false);

	for (int i = count (); i > 0; -- i)
		{
			QWidget *widget_p = widget (0);
			removeTab (0);
			delete widget_p;
		}

	setUpdatesEnabled (true);
}


uint32 ResultsWidget :: AddAllResultsPagesFromJSON (const json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s)
{
  uint32 num_pages = 0;

	ClearData ();

  /*
   * Avoid any repaints until we've added all of the pages
   */
  setUpdatesEnabled (false);

  if (json_is_array (json_p))
    {
      const size_t size = json_array_size (json_p);

      for (size_t i = 0; i < size; ++ i)
        {
          if (AddResultsPageFromJSON (json_array_get (json_p, i), service_name_s, service_description_s, service_uri_s))
            {
              ++ num_pages;
            }
        }   /* for (size_t i = 0; i < size; ++ i) */

    }   /*  if (json_is_array (json_p)) */
	else
		{
			if (AddResultsPageFromJSON (json_p, service_name_s, service_description_s, service_uri_s))
				{
					++ num_pages;
				}
		}

  setUpdatesEnabled (true);

  return num_pages;
}


bool ResultsWidget :: AddResultsPageFromJSON (const json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s)
{
	bool success_flag = false;
	json_t *results_json_p = json_object_get (json_p, SERVICE_RESULTS_S);

	#if RESULTS_WIDGET_DEBUG >= STM_LEVEL_FINER
	PrintJSONToLog (json_p, "result: ", STM_LEVEL_FINER, __FILE__, __LINE__);
	#endif

	if (results_json_p)
		{
			QWidget *page_p = CreatePageFromJSON (results_json_p, service_description_s, service_uri_s);

			if (page_p)
				{
					insertTab (0, page_p, service_name_s);
					success_flag = true;
				}
		}

	return success_flag;
}


bool ResultsWidget :: AddInterestedService (json_t *job_p, const char *service_name_s)
{
	int index = -1;

	for (int i = count () -1; i >= 0; -- i)
		{
			QString tab_text = tabText (i);

			if (tab_text.compare (RW_SERVICES_TAB_TITLE))
				{
					index = i;
					i = 0;
				}
		}

	if (index != -1)
		{
			QWidget *page_p = widget (index);
			QList l = page_p -> findChildren <ResultsList *> ();

			if (l.count () == 1)
				{
					ResultsList *results_p = l.at (0);

					results_p -> add

				}		/* if (l.count () == 1) */

		}		/* if (index != -1) */

}



QWidget *ResultsWidget :: CreatePageFromJSON (const json_t *results_json_p, const char * const description_s, const char * const uri_s)
{
	QWidget *page_p = new QWidget;
	ResultsList *list_p = new ResultsList (page_p);

	if (list_p -> SetListFromJSON (results_json_p))
		{
			QVBoxLayout *layout_p = new QVBoxLayout;
			page_p -> setLayout (layout_p);

			QLabel *label_p = new QLabel (QString (description_s), page_p);
			layout_p -> addWidget (label_p);

			if (uri_s)
				{
					QString s ("For more information, go to <a href=\"");
					s.append (uri_s);
					s.append ("\">");
					s.append (uri_s);
					s.append ("</a>");

					label_p = new QLabel (s, page_p);
					label_p -> setOpenExternalLinks (true);
					layout_p -> addWidget (label_p);
				}

			layout_p -> addWidget (list_p);
		}
	else
		{
			delete page_p;
			page_p = 0;
		}


	return page_p;
}
