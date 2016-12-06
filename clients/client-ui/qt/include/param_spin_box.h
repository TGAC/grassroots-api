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
#ifndef PARAMETER_SPIN_BOX_H
#define PARAMETER_SPIN_BOX_H


#include <QWidget>
#include <QSpinBox>

#include "parameter.h"
#include "base_param_widget.h"



class ParamSpinBox : public BaseParamWidget
{
	Q_OBJECT


public:
	ParamSpinBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, bool signed_flag, QWidget *parent_p = 0);
	virtual ~ParamSpinBox ();


	virtual bool SetValueFromText (const char *value_s);


	virtual bool SetValueFromJSON (const json_t * const value_p);

	virtual void SetDefaultValue ();
	virtual bool StoreParameterValue ();

protected:
	virtual QWidget *GetQWidget ();


	static int PSB_DEFAULT_MIN;

	static int PSB_DEFAULT_MAX;

private:
	QSpinBox *psb_spin_box_p;
	bool psb_signed_flag;
};


#endif		/* #ifndef PARAMETER_SPIN_BOX_H */
