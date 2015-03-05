#ifndef VIEWABLE_WIDGET_H
#define VIEWABLE_WIDGET_H

#include <QWidget>

class ViewableWidget
{
public:

	virtual const char *GetText () const = 0;
	virtual QWidget *GetWidget () = 0;

};

#endif // VIEWABLE_WIDGET_H
