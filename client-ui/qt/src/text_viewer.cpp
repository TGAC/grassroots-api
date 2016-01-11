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
#include <QFontDatabase>
#include <QActionGroup>


#include "text_viewer.h"

TextViewer :: TextViewer (QWidget *parent_p)
:	QTextEdit (parent_p)
{
}



const char *TextViewer :: GetText () const
{
	QString s = toPlainText ();
	QByteArray ba = s.toLocal8Bit ();
	const char *data_s = ba.constData ();

	return data_s;
}


QWidget *TextViewer :: GetWidget ()
{
	return this;
}


void TextViewer :: mouseReleaseEvent (QMouseEvent *event_p)
{
  QMenu menu;
  AddActions (menu);
  menu.exec (event_p -> globalPos ());
}

void TextViewer :: AddActions (QMenu &menu_r)
{
	// View Menu
	QMenu *menu_p = menu_r.addMenu (tr ("&View"));

	QMenu *sub_menu_p = new QMenu (tr ("Font"));
	QActionGroup *font_types_p = new QActionGroup (this);

	// System Font
	QAction *action_p = new QAction (tr ("System"), this);
	action_p -> setStatusTip (tr ("Use System Font"));
	action_p -> setChecked (true);
	action_p -> setCheckable (true);
	connect (action_p, &QAction :: triggered, this, &TextViewer :: SetSystemFont);
	sub_menu_p -> addAction (action_p);
	font_types_p -> addAction (action_p);

	// Fixed Font
	action_p = new QAction (tr ("Fixed"), this);
	action_p -> setStatusTip (tr ("Use Fixed Font"));
	action_p -> setCheckable (true);
	connect (action_p, &QAction :: triggered, this, &TextViewer :: SetFixedFont);
	sub_menu_p -> addAction (action_p);
	font_types_p -> addAction (action_p);


	menu_p -> addMenu (sub_menu_p);
}


void TextViewer :: SetSystemFont ()
{
	setFont (QFontDatabase :: systemFont (QFontDatabase :: GeneralFont));
}


void TextViewer :: SetFixedFont ()
{
	setFont (QFontDatabase :: systemFont (QFontDatabase :: FixedFont));
}
