#ifndef JSON_LIST_WIDGET_ITEM_H
#define JSON_LIST_WIDGET_ITEM_H

#include <QListWidgetItem>

#include "jansson.h"

class JSONListWidgetItem : public QListWidgetItem
{
public:
	JSONListWidgetItem (const QString &text_r, QListWidget *parent_p = 0, int type = UserType);

	void SetJSONData (const json_t *data_p);
	const json_t *GetJSONData () const;

private:
	const json_t *jlwi_json_data_p;
};

#endif // JSON_LIST_WIDGET_ITEM_H
