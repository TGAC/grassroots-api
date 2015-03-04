#ifndef JSON_VIEWER_H
#define JSON_VIEWER_H

#include <QTreeWidget>
#include <QTextEdit>

#include "jansson.h"

class JSONViewer
{
Q_OBJECT

public:
	JSONViewer (QWidget *parent_p = 0);

	void SetJSONData (const json_t *data_p);

private:
	QTreeWidget *jv_tree_p;
	QTextEdit *jv_viewer_p;
	const json_t *jv_data_p;
};

#endif // JSON_VIEWER_H
