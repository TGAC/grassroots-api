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
#ifndef PARAM_TEXT_BOX_H
#define PARAM_TEXT_BOX_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "parameter.h"

#include "base_param_widget.h"


class DroppableTextBox : public QPlainTextEdit
{
public:
	DroppableTextBox (QWidget *parent_p);

protected:
	virtual void dragEnterEvent (QDragEnterEvent *event_p);

	virtual void dropEvent (QDropEvent *event_p);

	void LoadText (const char *filename_s);
};


class ParamTextBox : public BaseParamWidget
{
	Q_OBJECT


public:
	ParamTextBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p = 0);
	virtual ~ParamTextBox ();

	virtual void RemoveConnection ();

	virtual void SetDefaultValue ();

	virtual bool SetValueFromText (const char *value_s);

	virtual bool StoreParameterValue ();

protected:
	DroppableTextBox *ptb_text_box_p;

	virtual QWidget *GetQWidget ();
};




#endif // PARAM_TEXT_BOX_H
