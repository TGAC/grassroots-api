#ifndef MODULE_PREFS_WIDGET_H
#define MODULE_PREFS_WIDGET_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QTextEdit>

#include "plugin.h"
#include "qt_parameter_widget.h"
#include "library_prefs_widget.h"
#include "prefs_widget.h"
#include "epr_qt_util_global.h"
#include "model_controller.h"
#include "module_selector.h"


class EPR_QT_UTILS_API ModulePrefsWidget : public BasePrefsWidget
{
	Q_OBJECT

private slots:
	void ChangeActiveModule (int index);
	void RefreshConfigValue ();

protected slots:
	virtual void CheckInterfaceLevel (ParameterLevel level);
	void SetActiveModuleFromConfiguration (const ConfigurationManager * const config_manager_p);

public:
	ModulePrefsWidget (QString &title_r, QString &path_r, const char * const config_key_s, const char * const plugin_s, const PluginType plugin_type, bool add_none_option_flag, const PrefsWidget * const prefs_widget_p, ModelController *controller_p, bool multi_select_flag);
	~ModulePrefsWidget ();

protected:
	ConfigWidget *GetModuleWidget (Module *module_p);
	char *GetActiveModuleConfigName (Module *module_p) const;

private:
	ModuleSelector *mpw_chooser_p;
	QStackedWidget *mpw_stacker_p;
	QLabel *mpw_module_title_p;
	QLabel *mpw_module_description_p;
	char *mpw_config_name_s;
};

#endif		/* #ifndef MODULE_PREFS_WIDGET_H */
