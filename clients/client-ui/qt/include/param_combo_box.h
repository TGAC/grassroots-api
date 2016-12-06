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
#ifndef PARAMETER_COMBO_BOX_H
#define PARAMETER_COMBO_BOX_H


#include <QWidget>
#include <QComboBox>
#include <QGroupBox>

#include "parameter.h"
#include "base_param_widget.h"



class ParamComboBox : public BaseParamWidget
{
	Q_OBJECT


public:
	static ParamComboBox *Create (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p = 0);
	ParamComboBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p = 0);
	virtual ~ParamComboBox ();


	virtual void SetDefaultValue ();

	virtual bool SetValueFromText (const char *value_s);

	virtual bool SetValueFromJSON (const json_t * const value_p);

	virtual bool StoreParameterValue ();

protected:
	virtual QWidget *GetQWidget ();

	bool AddOption (const SharedType *value_p, char *option_s, const SharedType * current_param_value_p);


private:
	QComboBox *pcb_combo_box_p;
	QGroupBox *pcb_group_p;
};


#endif		/* #ifndef PARAMETER_COMBO_BOX_H */
