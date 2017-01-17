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
#ifndef PROGRESS_WIDGET_H
#define PROGRESS_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QMovie>
#include <QPushButton>

#include "uuid/uuid.h"

#include "jansson.h"

#include "operation.h"


class ProgressWindow;

class ProgressWidget : public QWidget
{
public:
	static ProgressWidget *CreateProgressWidgetFromJSON (const json_t *json_p, ProgressWindow *parent_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s);

	~ProgressWidget ();

	const uuid_t *GetUUID () const;

	void SetStatus (OperationStatus status);

public slots:
	void ShowResults (bool checked_flag = false);

	void ShowErrors (bool checked_flag = false);

private:
//	QProgressBar *pw_progress_p;
	QLabel *pw_progress_label_p;
	QMovie *pw_anim_p;
	QLabel *pw_status_p;
	QLabel *pw_title_p;
	QLabel *pw_description_p;
	QPushButton *pw_results_button_p;
	uuid_t pw_id;
	ProgressWindow *pw_parent_p;
	json_t *pw_json_p;

	ProgressWidget (const json_t *json_p, const char *service_name_s, const char *service_description_s, ProgressWindow *parent_p);

	void GetServiceResults ();
};

#endif // PROGRESS_WIDGET_H
