#ifndef MATCHED_SERVICE_LIST_WIDGET_ITEM_H
#define MATCHED_SERVICE_LIST_WIDGET_ITEM_H

#include <QObject>

#include "json_list_widget_item.h"
#include "results_list.h"

class ServiceListWidgetItem : public JSONListWidgetItem, public QObject
{
	Q_OBJECT

public:
	ServiceListWidgetItem (const QString &text_r, QListWidget *parent_p = 0, int type = UserType);
	virtual ~ServiceListWidgetItem ();

	virtual void ShowData ();

signals:
	void ServiceRequested (const char *service_name_s, const json_t *params_json_p);

};

#endif // MATCHED_SERVICE_LIST_WIDGET_ITEM_H

