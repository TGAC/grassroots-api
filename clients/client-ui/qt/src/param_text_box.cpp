/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#include <stdio.h>

#include <QDebug>
#include <QFontMetrics>
#include <QMimeData>

#include "param_text_box.h"
#include "prefs_widget.h"

#include "string_utils.h"
#include "byte_buffer.h"


DroppableTextBox :: DroppableTextBox (QWidget *parent_p)
: QPlainTextEdit (parent_p)
{
	setAcceptDrops (true);

	/* Set tab width to 2 spaces */
	QFontMetrics metrics (font ());
	int space_width = metrics.width (' ');
	setTabStopWidth (2 * space_width);
}


DroppableTextBox ::	~DroppableTextBox ()
{}


void DroppableTextBox :: dropEvent (QDropEvent *event_p)
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

void DroppableTextBox :: dragEnterEvent (QDragEnterEvent *event_p)
{
	event_p -> acceptProposedAction ();
}


void DroppableTextBox :: LoadText (const char *filename_s)
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

									FreeLineBuffer (buffer_s);

								}
							else
							{
								loop_flag = false;
							}
						}

					if (success_flag)
						{
							const char *data_s = GetByteBufferData (buffer_p);

							if (!SetFromText (data_s))
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set text to %s", data_s);
								}
						}

					fclose (in_f);
				}

			FreeByteBuffer (buffer_p);
		}
}


bool DroppableTextBox :: SetFromText (const char * const data_s)
{
	clear ();
	insertPlainText (data_s);

	return true;
}


ParamTextBox :: ParamTextBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
:		BaseParamWidget (param_p, options_widget_p)
{
	CreateDroppableTextBox (parent_p);
}



bool ParamTextBox :: CreateDroppableTextBox (QWidget *parent_p)
{
	ptb_text_box_p = new DroppableTextBox (parent_p);

	return true;
}



ParamTextBox ::	~ParamTextBox ()
{
	delete ptb_text_box_p;
	ptb_text_box_p = NULL;
}





void ParamTextBox :: RemoveConnection ()
{
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



bool ParamTextBox :: StoreParameterValue ()
{
	QString s = ptb_text_box_p -> toPlainText ();
	QByteArray ba = s.toLocal8Bit ();
	const char *value_s = ba.constData ();

	bool success_flag = SetParameterValue (bpw_param_p, value_s, true);

	qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value_s << " " << success_flag;

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



bool ParamTextBox :: SetValueFromJSON (const json_t * const value_p)
{
	bool success_flag = false;

	if (json_is_string (value_p))
		{
			const char *value_s = json_string_value (value_p);

			ptb_text_box_p -> setPlainText (value_s);
			success_flag = true;
		}

	return success_flag;
}



