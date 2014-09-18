#ifndef BASE_PARAM_WIDGET_H
#define BASE_PARAM_WIDGET_H

#include <QWidget>

#include "parameter.h"


class PrefsWidget;

class BaseParamWidget : public QObject
{
	Q_OBJECT

public:
	BaseParamWidget (const Parameter * const param_p, const PrefsWidget * const prefs_widget_p);

	virtual ~BaseParamWidget ();


	virtual QWidget *GetQWidget () = 0;

	void CheckLevelDisplay (const ParameterLevel ui_level, QWidget *sync_widget_p, const QWidget * const parent_widget_p);

	virtual void RemoveConnection ();

protected:
	const Parameter * const bpw_param_p;
	const PrefsWidget * const bpw_prefs_widget_p;
	char *bpw_param_name_s;
};

#endif		/* #ifndef BASE_PARAM_WIDGET_H */
