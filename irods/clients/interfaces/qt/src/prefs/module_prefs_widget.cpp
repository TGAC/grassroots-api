#include <QDebug>
#include <QFormLayout>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "module_prefs_widget.h"
#include "epr_editor_interface.h"
#include "multi_select_module_selector.h"
#include "single_select_module_selector.h"

#include "filesystem.h"
#include "string_utils.h"
#include "plugin_manager.h"



#ifdef _DEBUG
	#define MODULE_PREFS_WIDGET_DEBUG (STM_LEVEL_FINEST)
#else
	#define MODULE_PREFS_WIDGET_DEBUG (STM_LEVEL_NONE)
#endif

ConfigWidget *ModulePrefsWidget :: GetModuleWidget (Module *module_p)
{
	ConfigWidget *widget_p = NULL;

	if (module_p)
		{
			/* check for a custom widget */
			widget_p = EprEditorInterface :: LoadPluginWidget (module_p, bpw_prefs_widget_p, bpw_controller_p);

			if (!widget_p)
				{
					const char *module_name_s = module_p -> mo_get_module_name_fn ();
					LinkedList *params_p = module_p -> mo_get_parameters_fn ();
					const ParameterLevel initial_level = bpw_prefs_widget_p -> GetCurrentParameterLevel ();

					#if MODULE_PREFS_WIDGET_DEBUG >= STM_LEVEL_FINEST
					if (params_p)
						{
							ParameterNode *node_p = reinterpret_cast <ParameterNode *> (params_p -> ll_head_p);

							PrintToOutputStream (NULL, ">>> %s\n", module_name_s);

							while (node_p)
								{
									PrintParameter (node_p -> pn_parameter_p, NULL);
									node_p = reinterpret_cast <ParameterNode *> (node_p -> pn_node.ln_next_p);
								}		/* while (node_p) */

						}		/* if (params_p) */
					#endif

					if (params_p)
						{
							widget_p = new QTParameterWidget (module_name_s, params_p, bpw_prefs_widget_p, initial_level);
						}
					else
						{
							widget_p = new ConfigWidget (module_name_s);
						}

				}		/* if (!widget_p) */

		}		/* if (module_p) */

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



ModulePrefsWidget :: ModulePrefsWidget (QString &title_r, QString &path_r, const char * config_key_s, const char * const plugin_s, const PluginType plugin_type, bool add_none_option_flag, const PrefsWidget * const prefs_widget_p, ModelController *controller_p, bool multi_select_flag)
: BasePrefsWidget (title_r, prefs_widget_p, controller_p),
	mpw_chooser_p (NULL),
	mpw_stacker_p (NULL),
	mpw_module_title_p (NULL),
	mpw_module_description_p (NULL),
	mpw_config_name_s (NULL)
{

	if (! (path_r.isNull () || path_r.isEmpty()))
		{
			mpw_config_name_s = CopyToNewString (config_key_s, 0, FALSE);

			QByteArray ba = path_r.toLocal8Bit ();
			const char *modules_root_path_s = ba.constData ();

			LinkedList *list_p = GetPlugins (modules_root_path_s, plugin_s, plugin_type);

			if (list_p)
				{
					/* Create a stack of widgets; 1 for each module of the plugin_type */
					mpw_stacker_p = new QStackedWidget;


					if (multi_select_flag)
						{
							mpw_chooser_p = new MultiSelectModuleSelector;
						}
					else
						{
							mpw_chooser_p = new SingleSelectModuleSelector;
						}

					mpw_module_title_p = new QLabel;
					mpw_module_description_p = new QLabel;

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
													mpw_stacker_p -> addWidget (module_widget_p);
													mpw_chooser_p -> AddModule (module_p);
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
							mpw_chooser_p -> AddModule (NULL);

							// add an empty widget
							QByteArray ba = ModuleSelector :: MS_NO_MODULE_SELECTED.toLocal8Bit ();
							const char *name_s = ba.constData ();

							mpw_stacker_p -> addWidget (new ConfigWidget (name_s));
						}

					/* Add the widgets */
					QVBoxLayout *layout_p = static_cast <QVBoxLayout *> (layout ());

					layout_p -> addWidget (mpw_chooser_p -> GetWidget ());

					QGroupBox *box_p = new QGroupBox;

					QFormLayout *info_layout_p = new QFormLayout;
					info_layout_p -> addRow (new QLabel ("Name:"), mpw_module_title_p);
					info_layout_p -> addRow (new QLabel ("Description:"), mpw_module_description_p);
					box_p -> setLayout (info_layout_p);

					layout_p -> addWidget (box_p);
					layout_p -> addWidget (mpw_stacker_p);

					/* When the combo box entry changes, change the visible widget */
					connect (mpw_chooser_p, &ModuleSelector :: ActiveIndexChanged, this, &ModulePrefsWidget :: ChangeActiveModule);

					connect (mpw_chooser_p, &ModuleSelector :: ValueChanged, this, &ModulePrefsWidget :: RefreshConfigValue);


					connect (prefs_widget_p, &PrefsWidget :: ConfigurationChanged, this, &ModulePrefsWidget :: SetActiveModuleFromConfiguration);

					FreeLinkedList (list_p);
				}		/* if (list_p) */
			else
				{
					qDebug () << " no modules at " << path_r;
				}

		}		/* if (path_s) */

	if (mpw_chooser_p)
		{
			ChangeActiveModule (mpw_chooser_p -> GetSelectedIndex ());
		}
}



void ModulePrefsWidget :: SetActiveModuleFromConfiguration (const ConfigurationManager * const config_manager_p)
{
	mpw_chooser_p -> SetActiveModuleFromConfiguration (config_manager_p, mpw_config_name_s);
}


/*
void ModulePrefsWidget :: SetActiveModuleToConfiguration (const ConfigurationManager * const config_manager_p)
{
	QVariant v = mpw_chooser_p -> itemData (mpw_chooser_p -> currentIndex ());
	Module *module_p = v.value <Module *> ();

	if (module_p)
		{
			const char *module_s = module_p -> mo_get_module_name_fn ();
			QString s (module_s);
			mpw_module_title_p -> setText (s);

			module_s = module_p -> mo_get_module_description_fn ();
			s = module_s;
			mpw_module_description_p -> setText (s);
		}
}
*/


//char *ModulePrefsWidget :: GetActiveModuleConfigName (const Module *module_p) const
//{
//	char *name_s = NULL;

//	if (module_p)
//		{
//			Plugin *plugin_p = module_p -> mo_plugin_p;

//			if (plugin_p)
//				{
//					char *plugin_name_s = GetPluginConfigName (plugin_p);

//					if (plugin_name_s)
//						{
//							if (strcmp (active_module_name_s, plugin_name_s) == 0)
//								{
//									mpw_chooser_p -> SetSelectedIndex (i);
//									i = -1;		/* force exit from loop */
//								}		/* if (strcmp (mcw_name_s, 0plugin_name_s) == 0) */

//						}		/* if (plugin_name_s) */

//				}		/* if (plugin_p) */

//		}		/* if (module_p) */

//}




ModulePrefsWidget :: ~ModulePrefsWidget ()
{
	if (mpw_config_name_s)
		{
			FreeCopiedString (mpw_config_name_s);
		}
}


void ModulePrefsWidget :: RefreshConfigValue ()
{
	ConfigurationManager *manager_p = bpw_controller_p -> GetConfigurationManager ();
	const char *config_value_s = mpw_chooser_p -> GetSelectorConfigurationValue ();

	if (config_value_s)
		{
			AddConfigurationValue (manager_p, mpw_config_name_s, config_value_s);
		}
	else
		{
			RemoveConfigurationValue (manager_p, mpw_config_name_s);
		}
}


void ModulePrefsWidget :: ChangeActiveModule (int index)
{
	const char *module_title_s = NULL;
	const char *module_description_s = NULL;

	Module *current_module_p = mpw_chooser_p -> GetActiveModule ();
	if (current_module_p)
		{
			module_title_s = current_module_p -> mo_get_module_name_fn ();
			module_description_s = current_module_p -> mo_get_module_name_fn ();

			if (!mpw_stacker_p -> isEnabled ())
				{
					mpw_stacker_p -> setEnabled (true);
				}

			mpw_stacker_p -> setCurrentIndex (index);
		}
	else
		{
			mpw_stacker_p -> setEnabled (false);
		}

	if (module_title_s)
		{
			QString s (module_title_s);
			mpw_module_title_p -> setText (s);
		}
	else
		{
			mpw_module_title_p -> clear ();
		}

	if (module_description_s)
		{
			QString s (module_description_s);
			mpw_module_description_p -> setText (s);
		}
	else
		{
			mpw_module_description_p -> clear ();
		}

	RefreshConfigValue ();
}


void ModulePrefsWidget :: CheckInterfaceLevel (ParameterLevel level)
{
	for (int i = mpw_stacker_p -> count () - 1; i >= 0; -- i)
		{
			ConfigWidget *params_widget_p = static_cast <ConfigWidget *> (mpw_stacker_p -> widget (i));

			params_widget_p -> UpdateParameterLevel (level, params_widget_p -> parentWidget ());

		}
}
