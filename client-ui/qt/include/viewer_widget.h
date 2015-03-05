#ifndef VIEWER_WIDGET_H
#define VIEWER_WIDGET_H

#include "viewable_widget.h"

class ViewerWidget : public QWidget
{
public:
	ViewerWidget (ViewableWidget *child_p, QWidget *parent_p = 0);

public slots:
	void Save (bool checked_flag);

private:
	ViewableWidget *vw_viewable_widget_p;
};

#endif // VIEWER_WIDGET_H
