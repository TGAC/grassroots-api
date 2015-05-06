#ifndef PROGRESS_WINDOW_H
#define PROGRESS_WINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "progress_widget.h"

class ProgressWindow : public QWidget
{
public:
	ProgressWindow (QMainWindow *parent_p);
	~ProgressWindow ();

	bool AddProgressItemFromJSON (const json_t *json_p);

protected:
	virtual void dropEvent (QDropEvent *event_p);
	virtual void dragEnterEvent (QDragEnterEvent *event_p);

private:
	QList <ProgressWidget *> pw_widgets;
};

#endif // PROGRESS_WINDOW_H
