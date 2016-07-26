/*
** Copyright 2014-2015 The Genome Analysis Centre
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
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QSaveFile>

#include "viewer_widget.h"


ViewerWidget :: ViewerWidget (ViewableWidget *child_p, QWidget *parent_p)
: vw_viewable_widget_p (child_p)
{
	QVBoxLayout *layout_p = new QVBoxLayout;

	QGroupBox *buttons_box_p = new QGroupBox;
	QHBoxLayout *buttons_layout_p = new QHBoxLayout;

	QPushButton *button_p = new QPushButton (QIcon ("images/ok"), tr ("Ok"), this);
	connect (button_p, &QPushButton :: clicked, this, &ViewerWidget :: close);
	buttons_layout_p -> addWidget (button_p);

	button_p = new QPushButton (QIcon ("images/save"), tr ("Save"), this);
	connect (button_p, &QPushButton :: clicked, this, &ViewerWidget :: Save);
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
