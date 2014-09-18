#include <QDebug>
#include <QFormLayout>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "string_utils.h"
#include "service_prefs_widget.h"


QTParameterWidget *ServicePrefsWidget :: GetServiceWidget (Service *service_p)
{
	QTParameterWidget *widget_p = NULL;

	if (service_p)
		{
				const char *module_name_s = service_p -> se_get_service_name_fn ();
				const char *description_s = service_p -> se_get_service_description_fn ();
				ParameterSet *params_p = service_p -> se_get_params_fn ();
				const ParameterLevel initial_level = PL_BASIC;

			if (params_p)
			{
				widget_p = new QTParameterWidget (module_name_s, description_s, params_p, NULL, initial_level);
			}

		}		/* if (service_p) */

	return widget_p;
}


//void stuff ()
//{
//	if (module_p -> mo_plugin_p)
//		{
//			if (config_key_s)
//				{
//					config_name_s = GetPluginConfigName (module_p -> mo_plugin_p);

//					if (config_name_s)
//						{
//							ConfigurationManager *config_manager_p = lpw_controller_p -> GetConfigurationManager ();

//							AddConfigurationValue (config_manager_p, config_key_s, config_name_s);
//							FreeCopiedString (config_name_s);
//						}		/* if (config_name_s) */

//				}		/* if (config_key_s) */
//		}
//}



ServicePrefsWidget :: ServicePrefsWidget (QString &title_r,const char *plugin_s, const PrefsWidget * const prefs_widget_p)
: BasePrefsWidget (title_r, prefs_widget_p),
	spw_chooser_p (NULL),
	spw_stacker_p (NULL),
	spw_module_title_p (NULL),
	spw_module_description_p (NULL),
	spw_config_name_s (NULL)
{

	if (! (path_r.isNull () || path_r.isEmpty()))
		{
			spw_config_name_s = CopyToNewString (config_key_s, 0, FALSE);

			QByteArray ba = path_r.toLocal8Bit ();
			const char *modules_root_path_s = ba.constData ();

			LinkedList *list_p = GetPlugins (modules_root_path_s, plugin_s, plugin_type);

			if (list_p)
				{
					/* Create a stack of widgets; 1 for each module of the plugin_type */
					spw_stacker_p = new QStackedWidget;


					if (multi_select_flag)
						{
							spw_chooser_p = new MultiSelectModuleSelector;
						}
					else
						{
							spw_chooser_p = new SingleSelectModuleSelector;
						}

					spw_module_title_p = new QLabel;
					spw_module_description_p = new QLabel;

					PluginListNode *node_p = (PluginListNode *) (list_p -> ll_head_p);
					int i = 0;

					while (node_p)
						{
							Plugin *plugin_p = node_p -> pln_plugin_p;
							Module *module_p = GetModuleFromPlugin (plugin_p, prefs_widget_p -> GetConfigurationManager ());

							if (module_p)
								{
									const char *module_name_s = module_p -> mo_get_module_name_fn ();

									if (module_name_s)
										{
											QWidget *module_widget_p = GetModuleWidget (module_p);

											if (module_widget_p)
												{
													spw_stacker_p -> addWidget (module_widget_p);
													spw_chooser_p -> AddModule (module_p);
												}		/* if (module_widget_p) */

										}		/* if (module_name_s) */
									else
										{

										}

								}		/* if (module_p) */
							else
								{

								}

							node_p = (PluginListNode *) (node_p -> pln_node.ln_next_p);
							++ i;
						}		/* while (node_p) */

					if (add_none_option_flag)
						{
							spw_chooser_p -> AddModule (NULL);

							// add an empty widget
							QByteArray ba = ModuleSelector :: MS_NO_MODULE_SELECTED.toLocal8Bit ();
							const char *name_s = ba.constData ();

							spw_stacker_p -> addWidget (new ConfigWidget (name_s));
						}

					/* Add the widgets */
					QVBoxLayout *layout_p = static_cast <QVBoxLayout *> (layout ());

					layout_p -> addWidget (spw_chooser_p -> GetWidget ());

					QGroupBox *box_p = new QGroupBox;

					QFormLayout *info_layout_p = new QFormLayout;
					info_layout_p -> addRow (new QLabel ("Name:"), spw_module_title_p);
					info_layout_p -> addRow (new QLabel ("Description:"), spw_module_description_p);
					box_p -> setLayout (info_layout_p);

					layout_p -> addWidget (box_p);
					layout_p -> addWidget (spw_stacker_p);

					/* When the combo box entry changes, change the visible widget */
					connect (spw_chooser_p, &ModuleSelector :: ActiveIndexChanged, this, &ServicePrefsWidget :: ChangeActiveModule);

					connect (spw_chooser_p, &ModuleSelector :: ValueChanged, this, &ServicePrefsWidget :: RefreshConfigValue);


					connect (prefs_widget_p, &PrefsWidget :: ConfigurationChanged, this, &ServicePrefsWidget :: SetActiveModuleFromConfiguration);

					FreeLinkedList (list_p);
				}		/* if (list_p) */
			else
				{
					qDebug () << " no modules at " << path_r;
				}

		}		/* if (path_s) */

	if (spw_chooser_p)
		{
			ChangeActiveModule (spw_chooser_p -> GetSelectedIndex ());
		}
}



ServicePrefsWidget :: ~ServicePrefsWidget ()
{
	if (spw_config_name_s)
		{
			FreeCopiedString (spw_config_name_s);
		}
}

