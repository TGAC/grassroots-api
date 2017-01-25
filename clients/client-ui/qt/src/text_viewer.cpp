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
#include <QFontDatabase>
#include <QActionGroup>
#include <QVBoxLayout>

#include "text_viewer.h"
#include "string_utils.h"


TextViewer :: TextViewer (QWidget *parent_p)
: QWidget (parent_p)
{
	QVBoxLayout *layout_p = new QVBoxLayout;

	tv_menubar_p = new QMenuBar (this);
	QMenu *menu_p = new QMenu ("View");
	AddActions (menu_p);
	tv_menubar_p -> addMenu (menu_p);
	layout_p -> addWidget (tv_menubar_p);

	tv_editor_p = new QPlainTextEdit;
	layout_p -> addWidget (tv_editor_p);

	setLayout (layout_p);

	setWindowTitle ("Text Viewer");

}


void TextViewer :: SetText (const char *value_s)
{
	tv_editor_p -> setPlainText (value_s);
}


 char *TextViewer :: GetText () const
{
	QString s = tv_editor_p -> toPlainText ();
	QByteArray ba = s.toLocal8Bit ();
	const char *data_s = ba.data ();

	return CopyToNewString (data_s, 0, false);
}


QWidget *TextViewer :: GetWidget ()
{
	return this;
}

void TextViewer :: AddActions (QMenu *menu_p)
{
	QMenu *sub_menu_p = new QMenu (tr ("Font"));
	QActionGroup *font_types_p = new QActionGroup (this);

	// Fixed Font
	QAction *action_p = new QAction (tr ("Fixed"), this);
	action_p -> setStatusTip (tr ("Use Fixed Font"));
	action_p -> setCheckable (true);
	connect (action_p, &QAction :: triggered, this, &TextViewer :: SetFixedFont);
	sub_menu_p -> addAction (action_p);
	font_types_p -> addAction (action_p);

	// System Font
	action_p = new QAction (tr ("System"), this);
	action_p -> setStatusTip (tr ("Use System Font"));
	action_p -> setCheckable (true);
	connect (action_p, &QAction :: triggered, this, &TextViewer :: SetSystemFont);
	sub_menu_p -> addAction (action_p);
	font_types_p -> addAction (action_p);
	action_p -> setChecked (true);


	menu_p -> addMenu (sub_menu_p);
}


void TextViewer :: SetSystemFont ()
{
	tv_editor_p -> setFont (QFontDatabase :: systemFont (QFontDatabase :: GeneralFont));
}


void TextViewer :: SetFixedFont ()
{
	tv_editor_p -> setFont (QFontDatabase :: systemFont (QFontDatabase :: FixedFont));
}
