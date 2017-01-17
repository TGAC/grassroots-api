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
#ifndef VIEWABLE_WIDGET_H
#define VIEWABLE_WIDGET_H

#include <QWidget>

class ViewableWidget
{
public:

	virtual char *GetText () const = 0;
	virtual QWidget *GetWidget () = 0;

};

#endif // VIEWABLE_WIDGET_H
