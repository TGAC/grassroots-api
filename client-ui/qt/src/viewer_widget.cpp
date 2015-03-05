#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QSaveFile>

#include "viewer_widget.h"


ViewerWidget :: ViewerWidget (ViewableWidget *child_p, QWidget *parent_p)
:
	vw_viewable_widget_p (child_p)
{
	QVBoxLayout *layout_p = new QVBoxLayout;

	QGroupBox *buttons_box_p = new QGroupBox;
	QHBoxLayout *buttons_layout_p = new QHBoxLayout;

	QPushButton *button_p = new QPushButton (QIcon ("images/save"), tr ("Save"), this);
	connect (button_p, &QPushButton :: clicked, this, &ViewerWidget :: Save);
	buttons_layout_p -> addWidget (button_p);

	button_p = new QPushButton (QIcon ("images/ok"), tr ("Ok"), this);

	connect (button_p, &QPushButton :: clicked, this, &ViewerWidget :: close);
	buttons_layout_p -> addWidget (button_p);

	buttons_box_p -> setLayout (buttons_layout_p);

	layout_p -> addWidget (vw_viewable_widget_p -> GetWidget ());
	layout_p -> addWidget (buttons_box_p);

	setLayout (layout_p);
}


void ViewerWidget :: Save (bool checked_flag)
{
	QString filename = QFileDialog :: getSaveFileName (this, tr ("Save File"));

	if (!filename.isNull ())
		{
			const char *data_s = vw_viewable_widget_p -> GetText ();

			if (data_s)
				{
					//QSaveFile
				}
			else
				{

				}
		}
}
