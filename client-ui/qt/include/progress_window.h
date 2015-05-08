#ifndef PROGRESS_WINDOW_H
#define PROGRESS_WINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "progress_widget.h"
#include "jansson.h"

class QTClientData;

class ProgressWindow : public QWidget
{
public:
	ProgressWindow (QMainWindow *parent_p, QTClientData *data_p);
	~ProgressWindow ();

	bool AddProgressItemFromJSON (const json_t *json_p);

	json_t *BuildStatusRequest ();

	json_t *BuildResultsRequest ();

protected:
	virtual void dropEvent (QDropEvent *event_p);
	virtual void dragEnterEvent (QDragEnterEvent *event_p);

private:
	QList <ProgressWidget *> pw_widgets;
	QTClientData *pw_data_p;

};

#endif // PROGRESS_WINDOW_H
