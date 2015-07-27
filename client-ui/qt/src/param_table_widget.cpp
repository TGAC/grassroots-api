#include "param_table_widget.h"

#include <stdio.h>

#include <QDebug>
#include <QMimeData>

#include "prefs_widget.h"

#include "string_utils.h"
#include "byte_buffer.h"


DroppableTableWidget :: DroppableTableWidget (QWidget *parent_p)
: QTableWidget (parent_p)
{
	setAcceptDrops (true);

}

void DroppableTableWidget :: dropEvent (QDropEvent *event_p)
{
	QList <QUrl> urls = event_p -> mimeData () -> urls ();

	if (! (urls.isEmpty ()))
		{
			QString filename = urls.first ().toLocalFile ();

			if (! ((filename.isEmpty ()) || (filename.isNull ())))
				{
					QByteArray ba = filename.toLocal8Bit ();
					const char * const filename_s = ba.constData ();

					qDebug () << "dropped " << filename;

					LoadText (filename_s);
				}		/* if (! (filename.isEmpty ())) */

		}		/* if (! (urls.isEmpty ())) */
}

void DroppableTableWidget :: dragEnterEvent (QDragEnterEvent *event_p)
{
	event_p -> acceptProposedAction ();
}


void DroppableTableWidget :: LoadText (const char *filename_s)
{
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
		{
			FILE *in_f = fopen (filename_s, "r");

			if (in_f)
				{
					bool loop_flag = true;
					bool success_flag = true;
					char *buffer_s = NULL;

					while (loop_flag)
						{
							if (GetLineFromFile (in_f, &buffer_s))
								{
									if (!AppendStringsToByteBuffer (buffer_p, buffer_s, "\n", NULL))
										{
											success_flag = false;
										}

								}
							else
							{
								loop_flag = false;
							}
						}

					if (success_flag)
					{
						const char *data_s = GetByteBufferData (buffer_p);

						clear();
						insertPlainText (data_s);
					}

					fclose (in_f);
				}

			FreeByteBuffer (buffer_p);
		}
}


ParamTableWidget :: ParamTableWidget (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
:		BaseParamWidget (param_p, options_widget_p)
{
	ptw_table_p = new DroppableTableWidget (parent_p);
}


ParamTableWidget ::	~ParamTableWidget ()
{
	delete ptw_table_p;
	ptw_table_p = NULL;
}


void ParamTableWidget :: RemoveConnection ()
{
}


void ParamTableWidget :: SetDefaultValue ()
{
	const char *value_s = bpw_param_p -> pa_default.st_string_value_s;
}


QWidget *ParamTableWidget :: GetQWidget ()
{
	return ptw_table_p;
}


void ParamTableWidget :: RefreshValue ()
{

}



void ParamTableWidget :: UpdateParameter ()
{
	const int num_rows = ptw_table_p -> rows ();
	const int num_cols = ptw_table_p -> cols ();

	for (int i = 0; i < num_rows; ++ i)
		{
			for (int i = 0; i < num_cols; ++ i)
				{

				}

		}


}


bool ParamTableWidget :: UpdateConfig ()
{
	return true;
}


bool ParamTableWidget :: UpdateConfigValue (const char * const value_s)
{
	bool success_flag = SetParameterValue (bpw_param_p, value_s);

	qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value_s;

	return success_flag;
}



bool ParamTableWidget :: SetValueFromText (const char *value_s)
{
	QString s = ptb_text_box_p -> toPlainText ();
	qDebug () << "old " << s;

	ptb_text_box_p -> setPlainText (value_s);

	s = ptb_text_box_p -> toPlainText ();
	qDebug () << "new " << s;

	return true;
}


