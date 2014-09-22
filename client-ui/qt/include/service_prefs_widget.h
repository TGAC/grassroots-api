#ifndef SERVICE_PREFS_WIDGET_H
#define SERVICE_PREFS_WIDGET_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QTextEdit>

#include "plugin.h"
#include "qt_parameter_widget.h"
#include "prefs_widget.h"


class ServicePrefsWidget : public QWidget
{
	Q_OBJECT

private slots:
	void ChangeActiveModule (int index);
	void RefreshConfigValue ();

protected slots:
	virtual void CheckInterfaceLevel (ParameterLevel level);

public:
	ServicePrefsWidget (QString &title_r, QString &path_r);
	~ServicePrefsWidget ();

protected:
	QTParameterWidget *GetServiceWidget (Service *service_p);

private:
	QLabel *mpw_module_title_p;
	QLabel *mpw_module_description_p;
	char *mpw_config_name_s;
};

#endif		/* #ifndef SERVICE_PREFS_WIDGET_H */
