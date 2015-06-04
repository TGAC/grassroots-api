#ifndef PROGRESS_WINDOW_H
#define PROGRESS_WINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QTimer>

#include "progress_widget.h"
#include "jansson.h"

class QTClientData;

class ProgressWindow : public QWidget
{
public:
	ProgressWindow (QMainWindow *parent_p, QTClientData *data_p);
	~ProgressWindow ();

	bool AddProgressItemFromJSON (const json_t *json_p);


	json_t *BuildResultsRequest ();

public slots:
	void UpdateStatuses ();
	void ViewResults ();
	void show ();

private:
	QTimer *pw_timer_p;
	bool pw_timer_started_flag;
	QList <ProgressWidget *> pw_widgets;
	QTClientData *pw_data_p;
};

#endif // PROGRESS_WINDOW_H
