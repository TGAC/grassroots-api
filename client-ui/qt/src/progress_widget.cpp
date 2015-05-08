#include <QHBoxLayout>
#include <QVBoxLayout>

#include "progress_widget.h"
#include "json_util.h"


ProgressWidget *ProgressWidget :: CreateProgressWidgetFromJSON (const json_t *json_p)
{
	ProgressWidget *widget_p = NULL;
	const char *uuid_s = GetJSONString (json_p, SERVICE_UUID_S);

	if (uuid_s)
		{
			uuid_t id;

			if (uuid_parse (uuid_s, id) == 0)
				{
					OperationStatus status;

					if (SetStatusFromJSON (json_p, &status))
						{
							const char *name_s = GetJSONString (json_p, JOB_NAME_S);
							const char *description_s = GetJSONString (json_p, JOB_DESCRIPTION_S);
						}

				}		/* if (uuid_parse (uuid_s, id) == 0) */

		}		/* if (uuid_s) */

	return widget_p;
}



ProgressWidget :: ProgressWidget (uuid_t id, OperationStatus status, const char *name_s, const char *description_s)
{
	QHBoxLayout *layout_p = new QHBoxLayout;

	uuid_copy (pw_id, id);

	QVBoxLayout *info_layout_p = new QVBoxLayout;

	pw_title_p = new QLabel (name_s);
	info_layout_p -> addWidget (pw_title_p);

	if (description_s)
		{
			pw_description_p = new QLabel (description_s);
			info_layout_p -> addWidget (pw_title_p);
		}

	layout_p -> addLayout (info_layout_p);

	pw_progress_p = new QProgressBar;
	pw_progress_p -> setMinimum (0);
	pw_progress_p -> setMaximum (0);
	layout_p -> addWidget (pw_progress_p);

	pw_status_p = new QLabel;
	SetStatus (status);
	layout_p -> addWidget (pw_status_p);

	setLayout (layout_p);
}


ProgressWidget ::	~ProgressWidget ()
{
}


const uuid_t *ProgressWidget ::	GetUUID () const
{
	return &pw_id;
}

void ProgressWidget :: SetStatus (OperationStatus status)
{
	const char *text_s = "";

	switch (status)
		{
			case OS_FAILED:
			case OS_FAILED_TO_START:
			case OS_ERROR:
				text_s = "Failed";
				break;

			case OS_IDLE:
				text_s = "Idle";
				break;

			case OS_PENDING:
				text_s = "Waiting to start";
				break;

			case OS_STARTED:
				text_s = "Started";
				break;

			case OS_FINISHED:
				text_s = "Finished";
				break;

			case OS_SUCCEEDED:
				text_s = "Succeeded";
				break;

			default:
				break;
		}


	pw_status_p -> setText (text_s);
}


bool ProgressWidget :: GetStatusFromJSON (const json_t *service_json_p, OperationStatus *status_p)
{
	bool success_flag = false;
	json_t *status_p = json_object_get (json_p, SERVICE_STATUS_S);

	if (status_p)
		{
			if (json_is_integer (status_p))
				{
					int i = json_integer_value (status_p);

					if ((i > OS_LOWER_LIMIT) && (i < OS_UPPER_LIMIT))
						{
							*status_p = (OperationStatus) i;

							success_flag = true;
						}		/* if ((i > OS_LOWER_LIMIT) && (i < OS_UPPER_LIMIT)) */

				}		/* if (json_is_integer (status_p)) */

		}		/* if (status_p) */

	return success_flag;
}
