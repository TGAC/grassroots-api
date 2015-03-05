#ifndef TEXT_VIEWER_H
#define TEXT_VIEWER_H

#include <QTextEdit>

#include "viewable_widget.h"

class TextViewer : public QTextEdit, public ViewableWidget
{
public:
	TextViewer (QWidget *parent_p = 0);

	virtual const char *GetText () const;
	virtual QWidget *GetWidget ();

private:
};

#endif // TEXT_VIEWER_H
