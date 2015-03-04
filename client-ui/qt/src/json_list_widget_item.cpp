#include "json_list_widget_item.h"


JSONListWidgetItem :: JSONListWidgetItem (const QString &text_r, QListWidget *parent_p, int type)
: QListWidgetItem (text_r, parent_p, type),
	jlwi_json_data_p (0)
{

}


void JSONListWidgetItem :: SetJSONData (const json_t *data_p)
{
	jlwi_json_data_p = data_p;
}



const json_t *JSONListWidgetItem :: GetJSONData () const
{
	return jlwi_json_data_p;
}

