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
#ifndef BASE_PARAM_WIDGET_H
#define BASE_PARAM_WIDGET_H

#include <QWidget>
#include <QLabel>

#include "parameter.h"
#include "jansson.h"

class PrefsWidget;

class BaseParamWidget : public QObject
{
	Q_OBJECT

public:
	BaseParamWidget (Parameter * const param_p, const PrefsWidget * const prefs_widget_p);

	virtual ~BaseParamWidget ();

	QWidget *GetUIQWidget ();


	QLabel *GetLabel () const;

	void CheckLevelDisplay (const ParameterLevel ui_level, const QWidget * const parent_widget_p);

	bool MeetsLevel (const ParameterLevel ui_level) const;

	virtual void RemoveConnection ();

	void SetVisible (const bool visible_flag);

	virtual void SetDefaultValue () = 0;

	virtual bool SetValueFromText (const char *value_s) = 0;

	virtual bool SetValueFromJSON (const json_t * const value_p) = 0;

	const char *GetParameterName () const;

	virtual bool StoreParameterValue () = 0;

	virtual void ShowErrors (const json_t *errors_p);

protected:
	Parameter * const bpw_param_p;
	const PrefsWidget * const bpw_prefs_widget_p;
	QLabel *bpw_label_p;
	char *bpw_param_name_s;

	virtual QWidget *GetQWidget () = 0;

};

#endif		/* #ifndef BASE_PARAM_WIDGET_H */
