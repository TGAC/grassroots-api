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
#ifndef TEXT_VIEWER_H
#define TEXT_VIEWER_H

#include <QTextEdit>
#include <QMouseEvent>
#include <QMenu>

#include "viewable_widget.h"

class TextViewer : public QTextEdit, public ViewableWidget
{
public:
	TextViewer (QWidget *parent_p = 0);

	virtual const char *GetText () const;
	virtual QWidget *GetWidget ();

protected:
	virtual void mouseReleaseEvent (QMouseEvent *event_p);

private:
	void AddActions (QMenu &menu_r);

	void SetSystemFont ();
	void SetFixedFont ();


};

#endif // TEXT_VIEWER_H
