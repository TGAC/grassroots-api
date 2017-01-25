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
#ifndef KEYWORD_WIDGET_H
#define KEYWORD_WIDGET_H

#include <QWidget>
#include <QLineEdit>

#include "parameter.h"

#include "runnable_widget.h"


class KeywordWidget : public QWidget, public RunnableWidget
{
	Q_OBJECT

public:
	KeywordWidget (QWidget *parent_p, ParameterLevel initial_level);

	virtual json_t *GetUserValuesAsJSON (bool full_flag);

signals:
	void RunKeywordSearch (QString s);

protected slots:
	void TextEntered ();

private:
	QLineEdit *kw_text_box_p;


};

#endif // KEYWORD_WIDGET_H
