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
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QDebug>
#include <QAction>
#include <QMenu>

#include "json_viewer.h"
#include "math_utils.h"
#include "string_utils.h"
#include "json_util.h"


Q_DECLARE_METATYPE (json_t *)


JSONViewer ::	JSONViewer (QWidget *parent_p)
: QWidget (parent_p)
{
	QHBoxLayout *layout_p = new QHBoxLayout;

	jv_tree_p = new QTreeWidget (this);
	jv_tree_p -> setContextMenuPolicy (Qt :: CustomContextMenu);
	connect (jv_tree_p, &QTreeWidget :: customContextMenuRequested, this, &JSONViewer :: PrepareMenu);

	layout_p -> addWidget (jv_tree_p);

	jv_viewer_p = new QTextEdit (this);
	jv_viewer_p -> setReadOnly (true);
	layout_p -> addWidget (jv_viewer_p);

	setLayout (layout_p);

	setWindowTitle ("JSON Viewer");
}


void JSONViewer :: PrepareMenu (const QPoint &pos_r)
{
	QTreeWidgetItem *item_p = jv_tree_p -> itemAt (pos_r);

	if (item_p)
		{
			QVariant v = item_p -> data (0, Qt :: DisplayRole);

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

							char *dump_s = json_dumps (json_p, 0);

							if (dump_s)
								{
									qDebug () << "tree has: " << endl << dump_s << endl;

									free (dump_s);
								}
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


const char *JSONViewer :: GetText () const
{
	QString s = jv_viewer_p ->  toPlainText ();
	QByteArray ba = s.toLocal8Bit ();

	return ba.constData ();
}


QWidget *JSONViewer :: GetWidget ()
{
	return this;
}


QTreeWidgetItem *JSONViewer :: InsertData (QTreeWidgetItem *parent_p, const char *key_s, json_t *data_p)
{
	QTreeWidgetItem *child_node_p = 0;

	if (parent_p)
		{
			child_node_p = new QTreeWidgetItem (parent_p);

			int num_children = parent_p -> childCount ();
			parent_p -> insertChild (num_children, child_node_p);
		}
	else
		{
			child_node_p = new QTreeWidgetItem;

			jv_tree_p -> addTopLevelItem (child_node_p);
		}

	if (key_s)
		{
			QString s = child_node_p -> text (0);

			if (s.isEmpty() || s.isNull ())
				{
					child_node_p -> setText (0, key_s);
				}
		}


	if (json_is_object (data_p))
		{
			json_t *child_json_p;
			const char *child_key_s;

			json_object_foreach (data_p, child_key_s, child_json_p)
				{
					InsertData (child_node_p, child_key_s, child_json_p);
				}
		}
	else if (json_is_array (data_p))
		{
			json_t *child_json_p;
			size_t i;

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
	else if (json_is_string (data_p))
		{
			const char *value_s = json_string_value (data_p);

			child_node_p -> setText (1, value_s);
		}
	else if (json_is_integer (data_p))
		{
			json_int_t i = json_integer_value (data_p);
			char *value_s = ConvertIntegerToString (i);

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

			if (value_s)
				{
					child_node_p -> setText (1, value_s);
					FreeCopiedString (value_s);
				}
		}
	else if (json_is_true (data_p))
		{
			child_node_p -> setText (1, "true");
		}
	else if (json_is_false (data_p))
		{
			child_node_p -> setText (1, "false");
		}

	return child_node_p;
}


void JSONViewer :: SetJSONData (json_t *data_p)
{
	jv_data_p = data_p;

	jv_tree_p -> clear ();
	jv_viewer_p -> clear ();

	jv_tree_p -> setColumnCount (2);

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

	char *value_s = json_dumps (data_p, 0);
	if (value_s)
		{
			jv_viewer_p -> setPlainText (value_s);
			free (value_s);
		}

	jv_tree_p -> update ();
}




void JSONViewer :: AddTopLevelNode (const char *key_s, json_t *data_p)
{
	QTreeWidgetItem *top_level_node_p = InsertData (NULL, key_s, data_p);

	if (top_level_node_p)
		{
			int i;
			QString services_name (LINKED_SERVICES_S);
			QTreeWidgetItem *services_node_p = 0;

			for (i = top_level_node_p -> childCount () - 1; i >= 0; -- i)
				{
					QTreeWidgetItem *child_node_p = top_level_node_p -> child (i);
					QString child_text = child_node_p -> text (0);

					if (services_name.compare (child_text) == 0)
						{
							services_node_p = child_node_p;
							i = -1;		/* force exit from loop */
						}

				}		/* for (i = top_level_node_p -> childCount (); i >= 0; -- i) */

			if (services_node_p)
				{
					json_t *services_json_p = json_object_get (data_p, LINKED_SERVICES_S);

					if (services_json_p)
						{
							if (json_is_array (services_json_p))
								{
									const size_t num_nodes = services_node_p -> childCount ();
									const size_t num_json_children = json_array_size (services_json_p);

									if (num_json_children == num_nodes)
										{
											for (i = 0; i < (int) num_nodes; ++ i)
												{
													QTreeWidgetItem *service_node_p = services_node_p -> child (i);
													json_t *service_json_p = json_array_get (services_json_p, i);

													const char *service_name_s = GetJSONString (service_json_p, SERVICE_NAME_S);
													QVariant var = QVariant :: fromValue (service_json_p);

													qDebug () << "service name " << service_name_s << endl;

													/*
													 * Set the node name to the service that it will run
													 */

													service_node_p -> setIcon (0, QIcon ("images/list_use"));
													if (service_name_s)
														{
															service_node_p -> setText (0, service_name_s);
														}

													service_node_p -> setData (0, Qt :: DisplayRole, var);

												}		/* for (i = top_level_node_p -> childCount (); i >= 0; -- i) */

										}


								}		/* if (json_is_array (services_json_p) */

						}		/* if (services_json_p) */

				}		/* if (services_node_p) */
		}

}




//QJsonDocument *ConvertToQJsonDocument (const json_t *json_p)
//{
//	char *dump_s = json_dumps (json_p, 0);

//	if (dump_s)
//		{
//			return QJsonDocument :: fromRawData (dump_s, )
//		}


//}



