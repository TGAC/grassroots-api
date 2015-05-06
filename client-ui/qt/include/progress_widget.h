#ifndef PROGRESS_WIDGET_H
#define PROGRESS_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>

#include "uuid/uuid.h"


class ProgressWidget : public QWidget
{
	ProgressWidget ();
	~ProgressWidget ();



private:
	QProgressBar *pw_progress_p;
	QLabel *pw_status_p;
	QLabel *pw_title_p;
	QLabel *pw_description_p;
	uuid_t pw_id;
};

#endif // PROGRESS_WIDGET_H
