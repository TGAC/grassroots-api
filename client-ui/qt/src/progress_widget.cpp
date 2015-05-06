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
					json_t *status_p = json_object_get (json_p, SERVICE_STATUS_S);

					if (status_p)
						{
							if (json_is_integer (status_p))
								{
									int i = json_integer_value (status_p);

									if ((i > OS_LOWER_LIMIT) && (i < OS_UPPER_LIMIT))
										{
											OperationStatus status = (OperationStatus) i;
											const char *name_s = GetJSONString (json_p, JOB_NAME_S);

											if (name_s)
												{
													const char *description_s = GetJSONString (json_p, JOB_DESCRIPTION_S);

													widget_p = new ProgressWidget (id, status, name_s, description_s);
												}		/* if (name_s) */

										}		/* if ((i > OS_LOWER_LIMIT) && (i < OS_UPPER_LIMIT)) */

								}		/* if (json_is_integer (status_p)) */

						}		/* if (status_p) */

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


	setLayout (layout_p);
}

ProgressWidget ::	~ProgressWidget ()
{
}


