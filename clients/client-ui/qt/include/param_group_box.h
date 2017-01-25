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
#ifndef PARAM_GROUP_BOX_H
#define PARAM_GROUP_BOX_H

#include <QFormLayout>
#include <QGroupBox>

#include "base_param_widget.h"


class ParamGroupBox : public QGroupBox
{
	Q_OBJECT

public:
	ParamGroupBox (const char * const name_s, const bool visible_flag);

	void AddParameterWidget (BaseParamWidget *widget_p);

	void CheckVisibility (ParameterLevel level);

protected:
	void paintEvent (QPaintEvent *event_p);

private:
	QList <BaseParamWidget *> pg_children;
	QFormLayout *pg_layout_p;

private slots:
	void ToggleCollapsed (bool checked);
};


#endif // PARAM_GROUP_BOX_H
