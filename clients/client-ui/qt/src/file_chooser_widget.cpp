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
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QGridLayout>

#include "file_chooser_widget.h"
#include "progress_window.h"
#include "prefs_widget.h"


void FileChooserWidget :: ChooseFile ()
{
	QFileDialog *dialog_p = new QFileDialog (fcw_widget_p, fcw_title, QDir :: currentPath ());

	dialog_p -> setFileMode (fcw_file_mode);


	if (fcw_file_mode == QFileDialog :: Directory)
		{
			dialog_p -> setOptions (QFileDialog :: ShowDirsOnly | QFileDialog :: HideNameFilterDetails);
		}
	else
		{
			dialog_p -> setOption (QFileDialog :: HideNameFilterDetails);
		}

	if (dialog_p -> exec ())
		{
			QStringList filenames = dialog_p -> selectedFiles ();

			if (! (filenames.isEmpty ()))
				{
					QString filename = filenames.at (0);

					if (fcw_chooser_p -> findText (filename) == -1)
						{
							fcw_chooser_p -> addItem (filename);
							fcw_chooser_p -> setCurrentIndex (fcw_chooser_p -> findText (filename));
						}		/* if (fcw_chooser_p -> findText (filename) == -1) */

				}		/* if (! (filename.isEmpty ())) */

		}		/* if (dialog_p -> exec ()) */

}


bool FileChooserWidget :: SetValueFromText (const char *value_s)
{
	fcw_chooser_p -> setCurrentText (value_s);

	return true;
}


bool FileChooserWidget :: SetValueFromJSON (const json_t * const value_p)
{
	bool success_flag = false;

	if (json_is_string (value_p))
		{
			const char *value_s = json_string_value (value_p);

			fcw_chooser_p -> setCurrentText (value_s);
			success_flag = true;
		}

	return success_flag;
}


void FileChooserWidget :: SetDefaultValue ()
{
	const Resource *resource_p = bpw_param_p -> pa_default.st_resource_value_p;

	if (resource_p)
		{
			fcw_chooser_p -> setCurrentText (resource_p -> re_value_s);

			if (resource_p -> re_protocol_s)
				{
					fcw_protocol_label_p -> setText (resource_p -> re_protocol_s);
					fcw_protocol_label_p -> show ();
				}
			else
				{
					fcw_protocol_label_p -> setText ("");
					fcw_protocol_label_p -> hide ();
				}
		}
}



bool FileChooserWidget :: StoreParameterValue ()
{
	QString value = fcw_chooser_p -> currentText ();
	QByteArray ba = value.toLocal8Bit ();
	const char * value_s = ba.constData ();

	return SetValueFromText (value_s);
}



FileChooserWidget :: FileChooserWidget (Parameter * const param_p, const PrefsWidget * const prefs_widget_p, QFileDialog :: FileMode mode)
	: BaseParamWidget (param_p, prefs_widget_p),
		fcw_file_mode (mode)
{
	fcw_chooser_p = new QComboBox;
	fcw_chooser_p -> setEditable (true);
	fcw_chooser_p -> setSizePolicy (QSizePolicy :: Expanding, QSizePolicy :: Preferred);

	QPushButton *browse_button_p = new QPushButton (tr ("Browse"));
	connect (browse_button_p, &QPushButton :: clicked, this, &FileChooserWidget :: ChooseFile);



	fcw_widget_p = new QWidget;
	QHBoxLayout *layout_p = new QHBoxLayout;


	fcw_protocol_label_p = new QLabel;
	fcw_protocol_label_p -> setBuddy (fcw_chooser_p);
	fcw_protocol_label_p -> setToolTip ("The protocol for the given file");

	/* don't show the protocol label when it's empty */
	fcw_protocol_label_p -> hide ();

	layout_p -> addWidget (fcw_protocol_label_p);
	layout_p -> addWidget (fcw_chooser_p);
	layout_p -> addWidget (browse_button_p);



	// Remove any borders top and bottom
	QMargins m = layout_p -> contentsMargins ();
	m.setLeft (0);
	m.setRight (0);
	layout_p -> setContentsMargins (m);

	fcw_widget_p -> setLayout (layout_p);
}


FileChooserWidget :: ~FileChooserWidget ()
{
	delete fcw_widget_p;
}




QWidget *FileChooserWidget :: GetQWidget ()
{
	return fcw_widget_p;
}
