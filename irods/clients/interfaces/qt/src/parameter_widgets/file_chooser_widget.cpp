#include <QFileDialog>
#include <QGroupBox>
#include <QGridLayout>

#include "file_chooser_widget.h"
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


bool FileChooserWidget :: UpdateConfig (const QString &value_r)
{
	QByteArray ba = value_r.toLocal8Bit ();
	const char *value_s = ba.constData ();
	
	return SetValue (value_s);
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

	connect (fcw_chooser_p, &QComboBox :: currentTextChanged, this, &FileChooserWidget :: UpdateConfig);


	fcw_widget_p = new QWidget;
	QHBoxLayout *layout_p = new QHBoxLayout;

	layout_p -> addWidget (fcw_chooser_p);
	layout_p -> addWidget (browse_button_p);

	// Remove any borders top and bottom
	int left;
	int top;
	int right;
	int bottom;

	layout_p -> getContentsMargins (&left, &top, &right, &bottom);
	layout_p -> setContentsMargins (left, 0, right, 0);

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
