#ifndef BASE_PARAM_WIDGET_H
#define BASE_PARAM_WIDGET_H

#include <QWidget>

#include "parameter.h"
#include "parameter_library_api.h"
#include "typedefs.h"

class PrefsWidget;

class WHEATIS_CLIENT_QT_LIBRARY_API BaseParamWidget : public QObject
{
	Q_OBJECT

public:
	BaseParamWidget (Parameter * const param_p, const PrefsWidget * const prefs_widget_p);

	virtual ~BaseParamWidget ();

//	virtual void UpdateWidgetFromConfig (const HashTable * const config_p) = 0;

	virtual QWidget *GetQWidget () = 0;

	void CheckLevelDisplay (const ParameterLevel ui_level, QWidget *sync_widget_p, const QWidget * const parent_widget_p);

	virtual void RemoveConnection ();


	bool SetValue (const void *value_p);

protected:
	Parameter * const bpw_param_p;
	const PrefsWidget * const bpw_prefs_widget_p;
	char *bpw_param_name_s;
};

#endif		/* #ifndef BASE_PARAM_WIDGET_H */
