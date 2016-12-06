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
	~DroppableTextBox ();

	void LoadText (const char *filename_s);

	virtual bool SetFromText (const char * const data_s);


protected:
	virtual void dragEnterEvent (QDragEnterEvent *event_p);

	virtual void dropEvent (QDropEvent *event_p);

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

	virtual bool SetValueFromJSON (const json_t * const value_p);

	virtual bool StoreParameterValue ();

protected:
	DroppableTextBox *ptb_text_box_p;

	virtual QWidget *GetQWidget ();

	virtual bool CreateDroppableTextBox (QWidget *parent_p);

};




#endif // PARAM_TEXT_BOX_H
