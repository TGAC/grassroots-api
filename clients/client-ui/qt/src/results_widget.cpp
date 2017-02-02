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
#include <QVBoxLayout>
#include <QLabel>

#include "results_widget.h"

#include "results_list.h"
#include "json_util.h"
#include "streams.h"


const char *ResultsWidget :: RW_SERVICES_TAB_TITLE_S = "Services";


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
			const char *job_name_s = GetJSONString (json_p, JOB_NAME_S);
			QWidget *page_p = CreatePageFromJSON (results_json_p, job_name_s, service_name_s, service_description_s, service_uri_s);

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
	bool success_flag = false;
	int index = -1;
	ResultsList *services_list_p = NULL;

	/* Find the "interested servces page" */
	for (int i = count () - 1; i >= 0; -- i)
		{
			QString tab_text = tabText (i);

			if (tab_text.compare (RW_SERVICES_TAB_TITLE_S))
				{
					index = i;
					i = 0;
				}
		}

	/*
	 * If we couldn't find  the "interested servces page",
	 * then create it
	 */
	if (index == -1)
		{
			ResultsPage *page_p = new ResultsPage (this);

			if (page_p)
				{
					insertTab (count (), page_p, RW_SERVICES_TAB_TITLE_S);
					services_list_p = page_p -> GetResultsList ();
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to insert %s page", RW_SERVICES_TAB_TITLE_S);
				}
		}
	else
		{
			ResultsPage *page_p = dynamic_cast <ResultsPage *> (widget (index));

			if (page_p)
				{
					services_list_p = page_p -> GetResultsList ();
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "page is not a ResultsPage");
				}

		}		/* if (index == -1) else */


	if (services_list_p)
		{
			if (services_list_p -> AddItemFromJSON (service_name_s, job_p))
				{
					success_flag = true;
				}
		}		/* if (services_list_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get %s page", RW_SERVICES_TAB_TITLE_S);
		}

	return success_flag;
}


bool ResultsWidget :: AddItemToResultsList (const json_t *results_json_p)
{
	return false;
}


void ResultsWidget :: SelectService (const char *service_name_s, const json_t *params_json_p)
{
	emit ServiceRequested (service_name_s, params_json_p);
}


void ResultsWidget :: RunService (json_t *service_json_p)
{
	emit RunServiceRequested (service_json_p);
}


ResultsPage *ResultsWidget :: CreatePageFromJSON (const json_t *results_json_p, const char * const job_name_s, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s)
{
	ResultsPage *page_p = 0;

	try
		{
			page_p = new ResultsPage (results_json_p, job_name_s, service_name_s, service_description_s, service_uri_s, this);
		}
	catch (std :: exception &ex_r)
		{
			PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, results_json_p, "Failed to create page");
		}

	return page_p;
}
