#include <QVBoxLayout>

#include "results_list.h"
#include "json_util.h"
#include "resource.h"

ResultsList :: ResultsList (QWidget *parent_p)
	:	QWidget (parent_p)
{
	QLayout *layout_p = new QVBoxLayout;

	rl_list_p = new QListWidget;

	layout_p -> addWidget (rl_list_p);

	setLayout (layout_p);
}


ResultsList :: ~ResultsList ()
{

}


bool ResultsList :: SetListFromJSON (const json_t *results_list_json_p)
{
	bool success_flag = false;

	if (json_is_array (results_list_json_p))
		{
			const size_t size = json_array_size (results_list_json_p);
			size_t count = 0;

			rl_list_p -> clear ();

			for (size_t i = 0; i < size; ++ i)
				{
					json_t *value_p = json_array_get (results_list_json_p, i);

					if (AddItemFromJSON (value_p))
						{
							++ count;
						}

				}		/* for (size_t i = 0; i < size; ++ i) */

			success_flag = (count == size);
		}		/* if (json_is_array (results_list_json_p)) */

	return success_flag;
}


bool ResultsList :: AddItemFromJSON (const json_t *resource_json_p)
{
	bool success_flag = false;
	const char *protocol_s = GetJSONString (resource_json_p, RESOURCE_PROTOCOL_S);

	if (protocol_s)
		{
			const char *value_s = GetJSONString (resource_json_p, RESOURCE_VALUE_S);
			const char *tooltip_s = GetJSONString (resource_json_p, RESOURCE_DESCRIPTION_S);

			if (value_s)
				{
					const char *icon_path_s = NULL;

					if ((strcmp (protocol_s, PROTOCOL_IRODS_S) == 0)  || (strcmp (protocol_s, PROTOCOL_FILE_S) == 0))
						{
							icon_path_s = "images/list_file";
						}
					else	if (strcmp (protocol_s, PROTOCOL_IRODS_S) == 0)
						{
							icon_path_s = "images/list_internet";
						}


					QListWidgetItem *item_p = new QListWidgetItem (value_s, rl_list_p);;

					if (icon_path_s)
						{
							item_p -> setIcon (QIcon (icon_path_s));
						}

					if (tooltip_s)
						{
							item_p -> setToolTip (tooltip_s);
						}

					success_flag = true;
				}		/* if (value_s) */

		}		/* if (protocol_s) */

	return success_flag;
}
