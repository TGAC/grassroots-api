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
#ifndef FILE_CHOOSER_WIDGET_H
#define FILE_CHOOSER_WIDGET_H

#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>

#include "base_param_widget.h"

class FileChooserWidget : public BaseParamWidget
{
	Q_OBJECT

private slots:
	void ChooseFile ();


public:
	FileChooserWidget (Parameter * const param_p, const PrefsWidget * const prefs_widget_p, QFileDialog :: FileMode mode);

	~FileChooserWidget ();

	virtual void SetDefaultValue ();

	virtual bool SetValueFromText (const char *value_s);

	virtual bool SetValueFromJSON (const json_t * const value_p);

	virtual bool StoreParameterValue ();

protected:
	virtual QWidget *GetQWidget ();

private:
	QWidget *fcw_widget_p;
	QComboBox *fcw_chooser_p;
	QLabel *fcw_protocol_label_p;
	QString fcw_title;
	QFileDialog :: FileMode fcw_file_mode;
	QFileDialog :: AcceptMode fcw_accept_mode;
};

#endif		/* #ifndef FILE_CHOOSER_WIDGET_H */
