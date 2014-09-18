#ifndef MODULE_PREFS_WIDGET_H
#define MODULE_PREFS_WIDGET_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QTextEdit>

#include "plugin.h"
#include "qt_parameter_widget.h"
#include "prefs_widget.h"

#include "service.h"
#include "base_prefs_widget.h"


class ServicePrefsWidget : public BasePrefsWidget
{
	Q_OBJECT

private slots:
	void ChangeActiveModule (int index);
	void RefreshConfigValue ();

protected slots:
	virtual void CheckInterfaceLevel (ParameterLevel level);

public:
	ServicePrefsWidget (QString &title_r, const char * const plugin_s, const PrefsWidget * const prefs_widget_p);


	~ServicePrefsWidget ();

protected:
	QTParameterWidget *GetServiceWidget (Service *service_p);

private:
	QStackedWidget *spw_stacker_p;
	QLabel *spw_module_title_p;
	QLabel *spw_module_description_p;
	char *spw_config_name_s;
};

#endif		/* #ifndef MODULE_PREFS_WIDGET_H */
