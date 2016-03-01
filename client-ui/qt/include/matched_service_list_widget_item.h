#ifndef MATCHED_SERVICE_LIST_WIDGET_ITEM_H
#define MATCHED_SERVICE_LIST_WIDGET_ITEM_H

#include "json_list_widget_item.h"

class ServiceListWidgetItem : public JSONListWidgetItem
{
public:
	ServiceListWidgetItem (const QString &text_r, QListWidget *parent_p = 0, int type = UserType);
	virtual ~ServiceListWidgetItem ();

	virtual void ShowData ();
};

#endif // MATCHED_SERVICE_LIST_WIDGET_ITEM_H

