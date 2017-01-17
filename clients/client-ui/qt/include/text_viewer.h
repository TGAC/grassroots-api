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

/**
 * @file
 * @brief
 */
#ifndef TEXT_VIEWER_H
#define TEXT_VIEWER_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QMouseEvent>
#include <QMenu>
#include <QMenuBar>

#include "viewable_widget.h"

class TextViewer : public QWidget, public ViewableWidget
{
public:
	TextViewer (QWidget *parent_p = 0);

	void SetText (const char *value_s);

	virtual char *GetText () const;
	virtual QWidget *GetWidget ();

public slots:
	void SetSystemFont ();
	void SetFixedFont ();



protected:
	QPlainTextEdit *tv_editor_p;
	QMenuBar *tv_menubar_p;




private:
	void AddActions (QMenu *menu_p);


};

#endif // TEXT_VIEWER_H
