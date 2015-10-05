/*
** Copyright 2014-2015 The Genome Analysis Centre
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
#include <QDesktopServices>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebView>
#include <QErrorMessage>
#include <QApplication>

#include "results_list.h"
#include "json_list_widget_item.h"
#include "text_viewer.h"
#include "viewer_widget.h"

#include "json_util.h"
#include "data_resource.h"



ResultsList :: ResultsList (QWidget *parent_p)
	:	QWidget (parent_p)
{
	QLayout *layout_p = new QVBoxLayout;

	rl_list_p = new QListWidget;

	connect (rl_list_p, &QListWidget :: itemDoubleClicked, this,  &ResultsList :: OpenItemLink);

	layout_p -> addWidget (rl_list_p);

	setLayout (layout_p);
}


ResultsList :: ~ResultsList ()
{
}


void ResultsList :: OpenItemLink (QListWidgetItem *item_p)
{
	JSONListWidgetItem *json_item_p = dynamic_cast <JSONListWidgetItem *> (item_p);

	if (json_item_p)
		{
			/* Open the json object */
			const json_t *data_p = json_item_p -> GetJSONData ();
			char *dump_s = json_dumps (data_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

			if (dump_s)
				{
					qDebug () << dump_s;
					free (dump_s);
				}

			if (json_is_string (data_p))
				{
					const char *data_s = json_string_value (data_p);
					TextViewer *text_viewer_p = new TextViewer;
					ViewerWidget *viewer_widget_p = new ViewerWidget (text_viewer_p, this);

					text_viewer_p -> setText (data_s);

/*
					viewer_widget_p -> adjustSize ();
					viewer_widget_p -> move (QApplication :: desktop () -> screen () -> rect ().center () - viewer_widget_p -> rect ().center ());
*/
					viewer_widget_p -> show ();
				}
			else
				{

				}
		}
	else
		{
			QVariant v = item_p -> data (Qt :: UserRole);
			QString s = v.toString ();
			QByteArray ba = s.toLocal8Bit ();
			const char *value_s = ba.constData ();

			if ((s.startsWith (PROTOCOL_IRODS_S))  || (s.startsWith (PROTOCOL_FILE_S)))
				{

				}
			else if ((s.startsWith (PROTOCOL_HTTP_S))  || (s.startsWith (PROTOCOL_HTTPS_S)))
				{
	/*
					QWebView *browser_p = new QWebView;

					browser_p -> load (QUrl (s));
					browser_p -> show ();
	*/
					if (!QDesktopServices :: openUrl (QUrl (s)))
						{
							QWebView *browser_p = new QWebView;

							rl_browsers.append (browser_p);
							browser_p -> load (QUrl (s));
							browser_p -> show ();
						}
				}
		}
}


bool ResultsList :: SetListFromJSON (const json_t *results_list_json_p)
{
	bool success_flag = false;

	if (json_is_array (results_list_json_p))
		{
			const size_t size = json_array_size (results_list_json_p);

			if (size > 0)
				{
					size_t count = 0;

					rl_list_p -> clear ();

					for (size_t i = 0; i < size; ++ i)
						{
							json_t *value_p = json_array_get (results_list_json_p, i);

							if (AddItemFromJSON (value_p))
								{
									++ count;
								}

						}		/* for (size_t i = 0; i < size; ++ i) */

					success_flag = (count == size);
				}

		}		/* if (json_is_array (results_list_json_p)) */

	return success_flag;
}


bool ResultsList :: AddItemFromJSON (const json_t *resource_json_p)
{
	bool success_flag = false;
	const char *protocol_s = GetJSONString (resource_json_p, RESOURCE_PROTOCOL_S);

	if (protocol_s)
		{
			const char *title_s = GetJSONString (resource_json_p, RESOURCE_TITLE_S);
			const char *description_s = GetJSONString (resource_json_p, RESOURCE_DESCRIPTION_S);			
			const char *icon_path_s = NULL;

			if (strcmp (protocol_s, PROTOCOL_INLINE_S) == 0)
				{
					json_t *data_p = json_object_get (resource_json_p, RESOURCE_DATA_S);

					if (data_p)
						{
							JSONListWidgetItem *item_p = new JSONListWidgetItem (title_s, rl_list_p);

							icon_path_s = "images/list_objects";

							item_p -> SetJSONData (data_p);

							if (!description_s)
								{
									description_s = "Click to view data";
								}

							item_p -> setToolTip (description_s);

							if (icon_path_s)
								{
									item_p -> setIcon (QIcon (icon_path_s));
								}

							success_flag = true;
						}
				}
			else
				{
					const char *value_s = GetJSONString (resource_json_p, RESOURCE_VALUE_S);

					if (value_s)
						{
							QListWidgetItem *item_p = 0;

							if (strcmp (protocol_s, PROTOCOL_FILE_S) == 0)
								{
									icon_path_s = "images/list_file";
								}
							else	if ((strcmp (protocol_s, PROTOCOL_HTTP_S) == 0) || (strcmp (protocol_s, PROTOCOL_HTTPS_S) == 0))
								{
									icon_path_s = "images/list_internet";
								}
							else if (strcmp (protocol_s, PROTOCOL_IRODS_S) == 0)
								{
									icon_path_s = "images/list_filelink";
								}

							item_p = new QListWidgetItem (title_s ? title_s : value_s, rl_list_p);

							QString s (protocol_s);
							s.append ("://");
							s.append (value_s);
							QVariant v (s);

							item_p -> setToolTip (description_s ? description_s : value_s);
							item_p -> setData (Qt :: UserRole, v);

							if (icon_path_s)
								{
									item_p -> setIcon (QIcon (icon_path_s));
								}

							success_flag = true;
						}
				}

		}		/* if (protocol_s) */

	return success_flag;
}
