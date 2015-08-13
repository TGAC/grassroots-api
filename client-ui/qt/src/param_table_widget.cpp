#include "param_table_widget.h"

#include <stdio.h>

#include <QDebug>
#include <QFont>
#include <QMimeData>
#include <QTableWidgetItem>

#include "prefs_widget.h"

#include "string_utils.h"
#include "byte_buffer.h"


DroppableTableWidget :: DroppableTableWidget (QWidget *parent_p, char row_delimiter, char column_delimter)
: QTableWidget (parent_p)
{
	setAcceptDrops (true);
	SetRowDelimiter (row_delimiter);
	SetColumnDelimiter (column_delimter);
	dtw_unpack_text_content_flag = true;
}


void DroppableTableWidget :: SetColumnDelimiter (char delimiter)
{
	dtw_column_delimiter = delimiter;
}


char DroppableTableWidget :: GetColumnDelimiter () const
{
	return dtw_column_delimiter;
}


void DroppableTableWidget :: SetRowDelimiter (char delimiter)
{
	dtw_row_delimiter = delimiter;
}


char DroppableTableWidget :: GetRowDelimiter () const
{
	return dtw_row_delimiter;
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

					qDebug () << "rows " << rowCount () << " cols " << columnCount ();

				}		/* if (! (filename.isEmpty ())) */

		}		/* if (! (urls.isEmpty ())) */
}

void DroppableTableWidget :: dragEnterEvent (QDragEnterEvent *event_p)
{
	event_p -> acceptProposedAction ();
	event_p -> accept ();
}

void DroppableTableWidget :: dragMoveEvent (QDragMoveEvent *event_p)
{
	event_p -> accept ();
}


bool DroppableTableWidget :: dropMimeData (int row, int column, const QMimeData *data_p, Qt :: DropAction action)
{
	return true;
}


void DroppableTableWidget :: SetRow (const int row, const char *data_s)
{
	const char *current_token_s = data_s;
	const char *next_token_s;
	int col = 0;
	bool loop_flag = true;
	bool valid_row_flag = false;

	if (row >= rowCount ())
		{
			setRowCount (row + 1);
		}

	while (loop_flag)
		{
			char *value_s = NULL;
			bool alloc_flag = false;

			if (*current_token_s != dtw_column_delimiter)
				{
					next_token_s = strchr (current_token_s, dtw_column_delimiter);

					if (next_token_s)
						{
							if (dtw_unpack_text_content_flag)
								{
									const char *first_value_p = current_token_s;
									bool found_flag = false;

									while ((!found_flag) && (first_value_p < next_token_s))
										{
											if (*first_value_p == '\"')
												{
													found_flag = true;
												}

											++ first_value_p;
										}

									if (found_flag)
										{
											const char *last_value_p = next_token_s;

											found_flag = false;

											while ((!found_flag) && (last_value_p > first_value_p))
												{
													if (*last_value_p == '\"')
														{
															found_flag = true;
														}
													else
														{
															-- last_value_p;
														}
												}

											if (found_flag)
												{
													value_s = CopyToNewString (first_value_p, last_value_p - first_value_p, false);
												}
										}

								}

							if (!value_s)
								{
									value_s = CopyToNewString (current_token_s, next_token_s - current_token_s, false);
								}

							if (value_s)
								{
									alloc_flag = true;
								}

							current_token_s = next_token_s + 1;


						}
					else
						{
							value_s = const_cast <char *> (current_token_s);
							loop_flag = false;
						}
				}
			else
				{
					++ current_token_s;
				}

			QTableWidgetItem *item_p = item (row, col);

			if (item_p)
				{
					item_p -> setText (value_s);
				}
			else
				{
					item_p = new QTableWidgetItem (value_s);

					if (col >= columnCount ())
						{
							setColumnCount (col + 1);
						}


					setItem (row, col, item_p);
				}


			if (row == 0)
				{
					QFont font;
					font.setBold (true);

					item_p -> setFont (font);
				}

			qDebug () << "num rows " << rowCount () << " num cols " << columnCount ();

			++ col;

			if (alloc_flag)
				{
					FreeCopiedString (value_s);
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
									success_flag = AppendToByteBuffer (buffer_p, &dtw_column_delimiter, 1);
								}

							if (!success_flag)
								{
									i = num_rows;
									j = num_cols;
								}
						}

					if (success_flag)
						{
							if (!AppendToByteBuffer (buffer_p, &dtw_row_delimiter, 1))
								{
									success_flag = false;
									i = num_rows;
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

	qDebug () << value_s << endl;
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

	qDebug () << "rows " << rowCount () << " cols " << columnCount ();

	if (in_f)
		{
			bool loop_flag = true;
			char *buffer_s = NULL;
			int row = 0;

			setSortingEnabled (false);

			while (loop_flag)
				{
					if (GetLineFromFile (in_f, &buffer_s))
						{
							if (*buffer_s)
								{
									SetRow (row, buffer_s);

									++ row;
								}
							else
								{
									loop_flag = false;
								}
						}
					else
					{
						loop_flag = false;
					}
				}

			setSortingEnabled (true);

			fclose (in_f);
		}

	qDebug () << "rows " << rowCount () << " cols " << columnCount ();
}


ParamTableWidget :: ParamTableWidget (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
:		BaseParamWidget (param_p, options_widget_p)
{
	ptw_table_p = new DroppableTableWidget (parent_p, '\n', '|');
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


