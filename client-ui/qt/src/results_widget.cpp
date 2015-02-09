
#include "results_widget.h"

#include "results_list.h"
#include "json_util.h"

ResultsWidget :: ResultsWidget (QWidget *parent_p)
 : QTabWidget (parent_p)
{
}

ResultsWidget :: ~ResultsWidget ()
{

}



uint32 ResultsWidget :: AddAllResultsPagesFromJSON (const json_t *json_p)
{
  uint32 num_pages = 0;

  /*
   * Avoid any repaints until we've added all of the pages
   */
  setUpdatesEnabled (false);

  if (json_is_array (json_p))
    {
      const size_t size = json_array_size (json_p);

      for (size_t i = 0; i < size; ++ i)
        {
          if (AddResultsPageFromJSON (json_array_get (json_p, i)))
            {
              ++ num_pages;
            }
        }   /* for (size_t i = 0; i < size; ++ i) */

    }   /*  if (json_is_array (json_p)) */


  setUpdatesEnabled (true);

  return num_pages;
}


bool ResultsWidget :: AddResultsPageFromJSON (const json_t *json_p)
{
	bool success_flag = false;
	json_t *results_json_p = json_object_get (json_p, SERVICE_RESULTS_S);

	if (results_json_p)
		{
			json_t *service_json_p = json_object_get (json_p, SERVICE_NAME_S);

			if (service_json_p)
				{
					if (json_is_string (service_json_p))
						{
							ResultsList *list_p = new ResultsList (this);

							if (list_p -> SetListFromJSON (results_json_p))
								{
									addTab (list_p, json_string_value (service_json_p));
									success_flag = true;
								}
							else
								{
									delete list_p;
								}
						}
				}
		}

	return success_flag;
}
