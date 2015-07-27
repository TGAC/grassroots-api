#include <stdio.h>

#include <QDebug>
#include <QMimeData>

#include "param_text_box.h"
#include "prefs_widget.h"

#include "string_utils.h"
#include "byte_buffer.h"

ParamTextBox :: ParamTextBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
:		BaseParamWidget (param_p, options_widget_p)
{
	ptb_text_box_p = new QPlainTextEdit (parent_p);

	connect (ptb_text_box_p, &QPlainTextEdit :: textChanged, this, &ParamTextBox :: UpdateConfig);

	ptb_text_box_p -> setAcceptDrops (true);
}


ParamTextBox ::	~ParamTextBox ()
{
	delete ptb_text_box_p;
	ptb_text_box_p = NULL;
}


void ParamTextBox :: RemoveConnection ()
{
	disconnect (ptb_text_box_p, &QPlainTextEdit :: textChanged, this, &ParamTextBox :: UpdateConfig);
}


void ParamTextBox :: SetDefaultValue ()
{
	const char *value_s = bpw_param_p -> pa_default.st_string_value_s;

	ptb_text_box_p -> setPlainText (value_s);
}


QWidget *ParamTextBox :: GetQWidget ()
{
	return ptb_text_box_p;
}


bool ParamTextBox :: UpdateConfig ()
{
	QString s = ptb_text_box_p -> toPlainText ();
	QByteArray ba = s.toLocal8Bit ();
	const char *value_s = ba.constData ();

	return UpdateConfigValue (value_s);
}


bool ParamTextBox :: UpdateConfigValue (const char * const value_s)
{
	bool success_flag = SetParameterValue (bpw_param_p, value_s);

	qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value_s;

	return success_flag;
}



bool ParamTextBox :: SetValueFromText (const char *value_s)
{
	QString s = ptb_text_box_p -> toPlainText ();
	qDebug () << "old " << s;

	ptb_text_box_p -> setPlainText (value_s);

	s = ptb_text_box_p -> toPlainText ();
	qDebug () << "new " << s;

	return true;
}


void ParamTextBox :: dropEvent (QDropEvent *event_p)
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

void ParamTextBox :: dragEnterEvent (QDragEnterEvent *event_p)
{
	event_p -> acceptProposedAction ();
}


void ParamTextBox :: LoadText (const char *filename_s)
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
									if (!AppendStringToByteBuffer (buffer_p, buffer_s))
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

						ptb_text_box_p -> clear();
						ptb_text_box_p -> insertPlainText (data_s);
					}

					fclose (in_f);
				}

			FreeByteBuffer (buffer_p);
		}
}




