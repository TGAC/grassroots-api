#include <QHBoxLayout>

#include "json_viewer.h"




JSONViewer ::	JSONViewer (QWidget *parent_p)
: QWidget (parent_p)
{
	QHBoxLayout *layout_p = new QHBoxLayout;

	jv_tree_p = new QTreeWidget (this);
	layout_p -> addWidget (jv_tree_p);

	jv_viewer_p = new QTextEdit (this);
	jv_viewer_p -> setReadOnly (true);
	layout_p -> addWidget (jv_viewer_p);

	setLayout (layout_p);
}



void JSONViewer :: SetJSONData (const json_t *data_p)
{
	size_t size = 1;

	jv_data_p = data_p;

	jv_viewer_p -> clear ();
	jv_viewer_p -> clear ();

	if (json_is_array (data_p))
		{
			size = json_array_size (data_p);
			jv_tree_p -> setColumnCount (size);
		}
	else
		{
			jv_tree_p -> setColumnCount (1);
		}


}
