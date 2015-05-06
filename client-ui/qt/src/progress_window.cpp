#include <QVBoxLayout>


#include "progress_widget.h"
#include "progress_window.h"

ProgressWindow :: ProgressWindow (QMainWindow *parent_p)
	: QWidget (parent_p)
{
	QVBoxLayout *layout_p = new QVBoxLayout;

	setLayout (layout_p);

	setAcceptDrops (true);
}


ProgressWindow ::	~ProgressWindow ()
{
}


bool ProgressWindow :: AddProgressItemFromJSON (const json_t *json_p)
{
	bool success_flag = false;
	ProgressWidget *widget_p = ProgressWidget :: CreateProgressWidgetFromJSON (json_p);

	if (widget_p)
		{
			pw_widgets.append (widget_p);
			layout () -> addWidget (widget_p);

			success_flag = true;
		}

	return success_flag;
}



void ProgressWindow :: dropEvent (QDropEvent *event_p)
{

}


void ProgressWindow :: dragEnterEvent (QDragEnterEvent *event_p)
{

}
