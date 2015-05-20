#include <QHBoxLayout>
#include <QVBoxLayout>

#include "progress_widget.h"
#include "json_util.h"
#include "json_tools.h"
#include "memory_allocations.h"


ProgressWidget *ProgressWidget :: CreateProgressWidgetFromJSON (const json_t *json_p, ProgressWindow *parent_p)
{
	ProgressWidget *widget_p = NULL;
	const char *uuid_s = GetJSONString (json_p, SERVICE_UUID_S);

	if (uuid_s)
		{
			uuid_t id;

			if (uuid_parse (uuid_s, id) == 0)
				{
					OperationStatus status;

					if (GetStatusFromJSON (json_p, &status))
						{
							const char *service_name_s = GetJSONString (json_p, JOB_SERVICE_S);
							const char *name_s = GetJSONString (json_p, JOB_NAME_S);
							const char *description_s = GetJSONString (json_p, JOB_DESCRIPTION_S);

							widget_p = new ProgressWidget (id, status, name_s, description_s, service_name_s, parent_p);
						}

				}		/* if (uuid_parse (uuid_s, id) == 0) */

		}		/* if (uuid_s) */

	return widget_p;
}



ProgressWidget :: ProgressWidget (uuid_t id, OperationStatus status, const char *name_s, const char *description_s, const char *service_name_s, ProgressWindow *parent_p)
{
	QHBoxLayout *layout_p = new QHBoxLayout;

	uuid_copy (pw_id, id);

	QVBoxLayout *info_layout_p = new QVBoxLayout;

	QString s = "<b>";
	s.append (service_name_s);
	s.append (" - ");
	s.append (name_s);
	s.append ("</b>");

	pw_title_p = new QLabel (s);
	info_layout_p -> addWidget (pw_title_p);

	if (description_s)
		{
			pw_description_p = new QLabel (description_s);
			info_layout_p -> addWidget (pw_description_p);
		}

	layout_p -> addLayout (info_layout_p);


	pw_progress_label_p = new QLabel;
	pw_anim_p = new QMovie ("images/anim_progress.gif");
	pw_progress_label_p -> setMovie (pw_anim_p);
	layout_p -> addWidget (pw_progress_label_p);

	/*
	pw_progress_p = new QProgressBar;
	pw_progress_p -> setMinimum (0);
	pw_progress_p -> setMaximum (0);
	layout_p -> addWidget (pw_progress_p);
	*/
	pw_status_p = new QLabel;
	layout_p -> addWidget (pw_status_p);


	pw_results_button_p = new QPushButton (tr ("View Results"));
	pw_results_button_p -> setEnabled (false);
	connect (pw_results_button_p, &QPushButton :: clicked, this, &ProgressWidget :: ShowResults);
	layout_p -> addWidget (pw_results_button_p);

	pw_parent_p = parent_p;


	SetStatus (status);

	setLayout (layout_p);
}


ProgressWidget ::	~ProgressWidget ()
{
}


const uuid_t *ProgressWidget ::	GetUUID () const
{
	return &pw_id;
}


void ProgressWidget ::	GetServiceResults ()
{

}


void ProgressWidget :: ShowResults (bool checked_flag)
{

}


void ProgressWidget :: SetStatus (OperationStatus status)
{
	const char *text_s = "";
	bool results_flag = false;

	switch (status)
		{
			case OS_FAILED:
			case OS_FAILED_TO_START:
			case OS_ERROR:
				text_s = "Failed";
				pw_anim_p -> stop ();
				break;

			case OS_IDLE:
				text_s = "Idle";
				break;

			case OS_PENDING:
				text_s = "Waiting to start";
				break;

			case OS_STARTED:
				text_s = "Started";
				pw_anim_p -> start ();
				break;

			case OS_FINISHED:
				text_s = "Finished";
				pw_anim_p -> stop ();
				results_flag = true;
				break;

			case OS_SUCCEEDED:
				text_s = "Succeeded";
				pw_anim_p -> stop ();
				results_flag = true;
				break;

			default:
				break;
		}


	pw_results_button_p -> setEnabled (results_flag);

	pw_status_p -> setText (text_s);
}

