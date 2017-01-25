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
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QDebug>
#include <QAction>
#include <QFontDatabase>
#include <QMenuBar>
#include <QTabWidget>


#include "json_viewer.h"
#include "math_utils.h"
#include "string_utils.h"
#include "json_util.h"
#include "streams.h"


#define JSON_VIEWER_DEBUG (STM_LEVEL_FINE)


Q_DECLARE_METATYPE (json_t *)


JSONViewer ::	JSONViewer (QWidget *parent_p)
: QWidget (parent_p)
{

	QVBoxLayout *layout_p = new QVBoxLayout;
	QMenuBar *menubar_p = new QMenuBar (this);
	QMenu *menu_p = new QMenu ("View");
	AddActions (menu_p);
	menubar_p -> addMenu (menu_p);
	layout_p -> addWidget (menubar_p);

	QTabWidget *tabs_p = new QTabWidget;


	jv_tree_p = new QTreeWidget (this);
	jv_tree_p -> setAlternatingRowColors (true);
	jv_tree_p -> setContextMenuPolicy (Qt :: CustomContextMenu);

	connect (jv_tree_p, &QTreeWidget :: customContextMenuRequested, this, &JSONViewer :: PrepareMenu);
	connect (jv_tree_p, &QTreeView :: expanded, this, &JSONViewer :: ResizeColumns);
	//connect (jv_tree_p, &QTreeView :: collapsed, this, &JSONViewer :: ResizeColumns);
	tabs_p -> addTab (jv_tree_p, "Document tree");

	jv_viewer_p = new QTextEdit (this);
	jv_viewer_p -> setReadOnly (true);
	tabs_p -> addTab (jv_viewer_p, "Document text");

	layout_p -> addWidget (tabs_p);
	setLayout (layout_p);

	setWindowTitle ("JSON Viewer");
}


void JSONViewer :: ResizeColumns (const QModelIndex &index)
{
	jv_tree_p -> resizeColumnToContents (0);
}


void JSONViewer :: PrepareMenu (const QPoint &pos_r)
{
	QTreeWidgetItem *item_p = jv_tree_p -> itemAt (pos_r);

	if (item_p)
		{
			QVariant v = item_p -> data (0, Qt :: UserRole);

			if (!v.isNull ())
				{
					json_t *json_p = v.value <json_t *> ();

					if (json_p)
						{
							QAction *run_action_p = new QAction (QIcon ("images/run"), tr ("&Run"), jv_tree_p);
							QString s ("Run ");

							s.append (item_p -> text (0));
							QByteArray ba = s.toLocal8Bit ();
							const char *value_s = ba.constData ();

							run_action_p -> setStatusTip (tr (value_s));

							QVariant var = QVariant :: fromValue (json_p);
							run_action_p -> setData (var);

							connect (run_action_p, &QAction :: triggered, this, &JSONViewer :: RunLinkedService);


							QMenu menu (this);
							menu.addAction (run_action_p);

							menu.exec (jv_tree_p -> mapToGlobal (pos_r));

							#if JSON_VIEWER_DEBUG >= STM_LEVEL_FINE
							PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, json_p, "service run json:\n");
							#endif

						}
				}

		}		/* if (item_p) */
}


void JSONViewer :: RunLinkedService (bool checked_flag)
{
	QAction *action_p = qobject_cast <QAction *> (sender ());

	if (action_p)
		{
			QVariant var = action_p -> data ();

			if (!var.isNull ())
				{
					json_t *json_p = var.value <json_t *> ();

					if (json_p)
						{
							emit RunServiceRequested (json_p);
						}
				}
		}

}


char *JSONViewer :: GetText () const
{
	QString s = jv_viewer_p ->  toPlainText ();
	QByteArray ba = s.toLocal8Bit ();
	const char *data_s = ba.constData ();
	return CopyToNewString (data_s, 0, false);
}


QWidget *JSONViewer :: GetWidget ()
{
	return this;
}


void JSONViewer :: AddActions (QMenu *menu_p)
{
	QMenu *sub_menu_p = new QMenu (tr ("Font"));
	QActionGroup *font_types_p = new QActionGroup (this);

	// Fixed Font
	QAction *action_p = new QAction (tr ("Fixed"), this);
	action_p -> setStatusTip (tr ("Use Fixed Font"));
	action_p -> setCheckable (true);
	connect (action_p, &QAction :: triggered, this, &JSONViewer :: SetFixedFont);
	sub_menu_p -> addAction (action_p);
	font_types_p -> addAction (action_p);

	// System Font
	action_p = new QAction (tr ("System"), this);
	action_p -> setStatusTip (tr ("Use System Font"));
	action_p -> setCheckable (true);
	connect (action_p, &QAction :: triggered, this, &JSONViewer :: SetSystemFont);
	sub_menu_p -> addAction (action_p);
	font_types_p -> addAction (action_p);
	action_p -> setChecked (true);


	menu_p -> addMenu (sub_menu_p);
}




QTreeWidgetItem *JSONViewer :: InsertData (QTreeWidgetItem *parent_p, const char *key_s, json_t *data_p)
{
	QTreeWidgetItem *child_node_p = 0;

	if (parent_p)
		{
			child_node_p = new QTreeWidgetItem (parent_p);

			parent_p -> addChild (child_node_p);
		}
	else
		{
			child_node_p = new QTreeWidgetItem (jv_tree_p);

			jv_tree_p -> addTopLevelItem (child_node_p);
		}

	if (key_s)
		{
			child_node_p -> setText (0, key_s);
		}


	if (json_is_object (data_p))
		{
			json_t *child_json_p;
			const char *child_key_s;

			child_node_p -> setIcon (0, QIcon ("images/braces"));

			json_object_foreach (data_p, child_key_s, child_json_p)
				{
					InsertData (child_node_p, child_key_s, child_json_p);
				}
		}
	else if (json_is_array (data_p))
		{
			json_t *child_json_p;
			size_t i;


			if (strcmp (key_s, LINKED_SERVICES_S) == 0)
				{
					json_array_foreach (data_p, i, child_json_p)
						{
							const char *service_name_s = GetJSONString (child_json_p, SERVICE_NAME_S);

							#if JSON_VIEWER_DEBUG >= STM_LEVEL_FINE
							qDebug () << "service name \"" << service_name_s << "\"" << endl;
							#endif

							QTreeWidgetItem *item_p = InsertData (child_node_p, service_name_s, child_json_p);

							if (item_p)
								{
									item_p -> setIcon (0, QIcon ("images/list_use"));

									if (service_name_s)
										{
											QString s ("Run ");
											s.append (service_name_s);

											item_p -> setText (0, s);
										}

									QVariant var = QVariant :: fromValue (child_json_p);
									item_p -> setData (0, Qt :: UserRole, var);
								}		/* if (item_p) */

						}		/* json_array_foreach (data_p, i, child_json_p) */

				}		/* if (strcmp (key_s, LINKED_SERVICES_S) == 0) */
			else
				{

					child_node_p -> setIcon (0, QIcon ("images/brackets"));

					json_array_foreach (data_p, i, child_json_p)
						{
							char *value_s = ConvertIntegerToString (i);

							if (value_s)
								{
									InsertData (child_node_p, value_s, child_json_p);

									FreeCopiedString (value_s);
								}
						}

				}

		}
	else if (json_is_string (data_p))
		{
			const char *value_s = json_string_value (data_p);

			child_node_p -> setIcon (0, QIcon ("images/text"));
			child_node_p -> setText (1, value_s);
		}
	else if (json_is_integer (data_p))
		{
			json_int_t i = json_integer_value (data_p);
			char *value_s = ConvertIntegerToString (i);

			child_node_p -> setIcon (0, QIcon ("images/numberdecmark"));

			if (value_s)
				{
					child_node_p -> setText (1, value_s);
					FreeCopiedString (value_s);
				}
		}
	else if (json_is_real (data_p))
		{
			double d = json_real_value (data_p);
			char *value_s = ConvertDoubleToString (d);

			child_node_p -> setIcon (0, QIcon ("images/numberdec"));

			if (value_s)
				{
					child_node_p -> setText (1, value_s);
					FreeCopiedString (value_s);
				}
		}
	else if (json_is_true (data_p))
		{
			child_node_p -> setText (1, "true");
			child_node_p -> setIcon (0, QIcon ("images/ok"));
		}
	else if (json_is_false (data_p))
		{
			child_node_p -> setText (1, "false");
			child_node_p -> setIcon (0, QIcon ("images/cancel"));
		}

	return child_node_p;
}


void JSONViewer :: SetJSONData (json_t *data_p)
{
	jv_data_p = data_p;

	jv_tree_p -> clear ();
	jv_viewer_p -> clear ();


	jv_tree_p -> setColumnCount (2);

	QStringList labels;
	labels << tr ("Key") << tr ("Value");
	jv_tree_p -> setHeaderLabels (labels);

	if (json_is_array (data_p))
		{
			size_t i;
			json_t *item_p;

			json_array_foreach (data_p, i, item_p)
				{
					char *value_s = ConvertIntegerToString (i);

					AddTopLevelNode (value_s, item_p);

					if (value_s)
						{
							FreeCopiedString (value_s);
						}
				}
		}
	else
		{
			AddTopLevelNode ("0", data_p);
		}

	char *value_s = json_dumps (data_p, JSON_INDENT (2));
	if (value_s)
		{
			jv_viewer_p -> setPlainText (value_s);
			free (value_s);
		}

	jv_tree_p -> resizeColumnToContents (1);

	jv_tree_p -> repaint ();
}




void JSONViewer :: AddTopLevelNode (const char *key_s, json_t *data_p)
{
	QTreeWidgetItem *top_level_node_p = InsertData (NULL, key_s, data_p);

}




void JSONViewer :: SetSystemFont ()
{
	QFont f = QFontDatabase :: systemFont (QFontDatabase :: GeneralFont);

	jv_viewer_p -> setFont (f);
	jv_tree_p -> setFont (f);
}


void JSONViewer :: SetFixedFont ()
{
	QFont f = QFontDatabase :: systemFont (QFontDatabase :: FixedFont);

	jv_viewer_p -> setFont (f);
	jv_tree_p -> setFont (f);
}

