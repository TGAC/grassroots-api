#include "matched_service_list_widget_item.h"


ServiceListWidgetItem :: ServiceListWidgetItem (const QString &text_r, QListWidget *parent_p, int type)
: JSONListWidgetItem (text_r, parent_p, type)
{
}


ServiceListWidgetItem :: ~ServiceListWidgetItem ()
{
}


void ServiceListWidgetItem :: ShowData ()
{
	/*
	 * Load the parameters into the appropraite Service
	 * in the prefs window and display it
	 */
}
