#ifndef PROGRESS_WIDGET_H
#define PROGRESS_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QMovie>
#include <QPushButton>

#include "uuid/uuid.h"

#include "jansson.h"

#include "operation.h"


class ProgressWindow;

class ProgressWidget : public QWidget
{
public:
	static ProgressWidget *CreateProgressWidgetFromJSON (const json_t *json_p, ProgressWindow *parent_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s);

	~ProgressWidget ();

	const uuid_t *GetUUID () const;

	void SetStatus (OperationStatus status);

public slots:
	void ShowResults (bool checked_flag = false);

private:
//	QProgressBar *pw_progress_p;
	QLabel *pw_progress_label_p;
	QMovie *pw_anim_p;
	QLabel *pw_status_p;
	QLabel *pw_title_p;
	QLabel *pw_description_p;
	QPushButton *pw_results_button_p;
	uuid_t pw_id;
	ProgressWindow *pw_parent_p;


	ProgressWidget (uuid_t id, OperationStatus status, const char *name_s, const char *description_s, const char *service_name_s, ProgressWindow *parent_p);

	void GetServiceResults ();
};

#endif // PROGRESS_WIDGET_H
