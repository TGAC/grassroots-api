#include <QVBoxLayout>
#include <QLabel>

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



uint32 ResultsWidget :: AddAllResultsPagesFromJSON (const json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s)
{
  uint32 num_pages = 0;

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
	char *dump_s = json_dumps (json_p, JSON_INDENT (2));
	json_t *results_json_p = json_object_get (json_p, SERVICE_RESULTS_S);

	if (results_json_p)
		{
			QWidget *page_p = CreatePageFromJSON (results_json_p, service_description_s, service_uri_s);

			if (page_p)
				{
					addTab (page_p, service_name_s);
					success_flag = true;
				}
		}

	return success_flag;
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
