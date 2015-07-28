#ifndef BASE_PARAM_WIDGET_H
#define BASE_PARAM_WIDGET_H

#include <QWidget>
#include <QLabel>

#include "parameter.h"


class PrefsWidget;

class BaseParamWidget : public QObject
{
	Q_OBJECT

public:
	BaseParamWidget (Parameter * const param_p, const PrefsWidget * const prefs_widget_p);

	virtual ~BaseParamWidget ();

	QWidget *GetUIQWidget ();


	QLabel *GetLabel () const;

	void CheckLevelDisplay (const ParameterLevel ui_level, const QWidget * const parent_widget_p);

	bool MeetsLevel (const ParameterLevel ui_level) const;

	virtual void RemoveConnection ();

	void SetVisible (const bool visible_flag);

	virtual void SetDefaultValue () = 0;

	virtual bool SetValueFromText (const char *value_s) = 0;

	const char *GetParameterName () const;

	virtual bool StoreParameterValue () = 0;

protected:
	Parameter * const bpw_param_p;
	const PrefsWidget * const bpw_prefs_widget_p;
	QLabel *bpw_label_p;
	char *bpw_param_name_s;

	virtual QWidget *GetQWidget () = 0;

};

#endif		/* #ifndef BASE_PARAM_WIDGET_H */
