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
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include "keyword_widget.h"


KeywordWidget :: KeywordWidget (QWidget *parent_p, ParameterLevel initial_level)
	: QWidget (parent_p)
{
	QVBoxLayout *layout_p = new QVBoxLayout;

	QLabel *label_p = new QLabel ("Choose your search terms");
	layout_p -> addWidget (label_p);

	QHBoxLayout *controls_layout_p = new QHBoxLayout;

	kw_text_box_p = new QLineEdit;
	connect (kw_text_box_p, &QLineEdit :: returnPressed, this, &KeywordWidget :: TextEntered);
	controls_layout_p -> addWidget (kw_text_box_p);

	QPushButton *button_p = new QPushButton (this);
	button_p -> setIcon (QIcon ("images/search"));
	connect (button_p, &QAbstractButton :: clicked, this, &KeywordWidget :: TextEntered);
	controls_layout_p -> addWidget (button_p);

	layout_p -> addLayout (controls_layout_p);
	layout_p -> setAlignment (Qt :: AlignCenter);

	setLayout (layout_p);
}


void KeywordWidget :: TextEntered ()
{
	QString search_text = kw_text_box_p -> text ();
	emit RunKeywordSearch (search_text);
}


json_t *KeywordWidget :: GetUserValuesAsJSON (bool full_flag)
{
	return NULL;
}
