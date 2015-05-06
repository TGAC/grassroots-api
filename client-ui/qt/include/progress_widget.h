#ifndef PROGRESS_WIDGET_H
#define PROGRESS_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>

#include "uuid/uuid.h"

#include "jansson.h"

#include "operation.h"



class ProgressWidget : public QWidget
{
public:
	static ProgressWidget *CreateProgressWidgetFromJSON (const json_t *json_p);

	~ProgressWidget ();


private:
	QProgressBar *pw_progress_p;
	QLabel *pw_status_p;
	QLabel *pw_title_p;
	QLabel *pw_description_p;
	uuid_t pw_id;

	ProgressWidget (uuid_t id, OperationStatus status, const char *name_s, const char *description_s);

};

#endif // PROGRESS_WIDGET_H
