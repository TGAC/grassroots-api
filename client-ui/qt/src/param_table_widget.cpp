#include "param_table_widget.h"

#include <stdio.h>

#include <QDebug>
#include <QMimeData>
#include <QTableWidgetItem>

#include "prefs_widget.h"

#include "string_utils.h"
#include "byte_buffer.h"


DroppableTableWidget :: DroppableTableWidget (QWidget *parent_p, char delimiter)
: QTableWidget (parent_p)
{
	setAcceptDrops (true);
	SetDelimiter (delimiter);
}


void DroppableTableWidget :: SetDelimiter (char delimiter)
{
	dtw_delimiter = delimiter;
}


char DroppableTableWidget :: GetDelimiter ()
{
	return dtw_delimiter;
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


void DroppableTableWidget :: SetRow (const int row, const char *data_s)
{
	const char *current_token_s = data_s;
	const char *next_token_s;
	int col = 0;
	bool loop_flag = true;

	while (loop_flag)
		{
			char *value_s = NULL;
			bool alloc_flag = false;

			next_token_s = strchr (current_token_s, dtw_delimiter);

			if (next_token_s)
				{
					value_s = CopyToNewString (current_token_s, next_token_s - current_token_s, false);

					if (value_s)
						{
							alloc_flag = true;
						}
					current_token_s = next_token_s;
					++ col;
				}
			else
				{
					value_s = const_cast <char *> (current_token_s);
					loop_flag = false;
				}

			QTableWidgetItem *item_p = item (row, col);

			if (item_p)
				{
					item_p -> setText (value_s);
				}
			else
				{
					item_p = new QTableWidgetItem (value_s);
					setItem (row, col, item_p);
				}
		}
}


char *DroppableTableWidget :: GetValueAsText ()
{
	char *value_s = NULL;
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
		{
			const int num_rows = rowCount ();
			const int num_cols = columnCount ();
			bool success_flag = true;

			for (int i = 0; i < num_rows; ++ i)
				{
					for (int j = 0; j < num_cols; ++ j)
						{
							QTableWidgetItem *item_p = item (i, j);

							if (item_p)
								{
									QString s = item_p -> text ();
									QByteArray ba = s.toLocal8Bit ();
									const char *item_value_s = ba.constData ();

									success_flag = AppendStringToByteBuffer (buffer_p, item_value_s);
								}

							if (success_flag)
								{
									success_flag = AppendToByteBuffer (buffer_p, &dtw_delimiter, 1);
								}

							if (!success_flag)
								{
									i = num_rows;
									j = num_cols;

								}
						}
				}

			if (success_flag)
				{
					const char *data_s = GetByteBufferData (buffer_p);

					value_s = CopyToNewString (data_s, 0, false);
				}

			FreeByteBuffer (buffer_p);
		}

	return value_s;
}



bool DroppableTableWidget :: SetValueFromText (const char *data_s)
{
	bool success_flag = false;

	return success_flag;
}


void DroppableTableWidget :: LoadText (const char *filename_s)
{
	FILE *in_f = fopen (filename_s, "r");

	if (in_f)
		{
			bool loop_flag = true;
			char *buffer_s = NULL;
			int row = 0;

			while (loop_flag)
				{
					if (GetLineFromFile (in_f, &buffer_s))
						{
							SetRow (row, buffer_s);

							++ row;
						}
					else
					{
						loop_flag = false;
					}
				}

			fclose (in_f);
		}
}


ParamTableWidget :: ParamTableWidget (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
:		BaseParamWidget (param_p, options_widget_p)
{
	ptw_table_p = new DroppableTableWidget (parent_p, '|');
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


bool ParamTableWidget :: StoreParameterValue ()
{
	bool success_flag  = false;
	char *value_s = ptw_table_p ->  GetValueAsText ();

	if (value_s)
		{
			SetParameterValue (bpw_param_p, value_s);

			success_flag = true;
			FreeCopiedString (value_s);
		}

	return success_flag;
}



bool ParamTableWidget :: SetValueFromText (const char *value_s)
{
	bool success_flag  = false;
	const char *current_row_s = value_s;
	const char *next_row_s;
	int row = 0;

	while ((next_row_s = strchr (current_row_s, '\n')) != NULL)
		{
			char *row_s = CopyToNewString (current_row_s, next_row_s - current_row_s, false);

			if (row_s)
				{
					ptw_table_p -> SetRow (row, row_s);
					FreeCopiedString (row_s);
				}

			current_row_s = next_row_s;
			++ row;
		}

	return success_flag;

}


