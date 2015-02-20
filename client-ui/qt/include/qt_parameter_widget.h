#ifndef QT_PARAMETER_WIDGET_H
#define QT_PARAMETER_WIDGET_H

#include <QFormLayout>
#include <QHash>
#include <QWidget>
#include <QCheckBox>
#include <QWebView>
#include <QList>

#include "parameter.h"
#include "parameter_set.h"
#include "base_param_widget.h"

// forward class declaration
class PrefsWidget;



class  QTParameterWidget : public QWidget
{
	Q_OBJECT


public:
	QTParameterWidget (const char *name_s, const char * const description_s, const char * const uri_s, ParameterSet *parameters_p, const PrefsWidget * const prefs_widget_p, const ParameterLevel initial_level);

	~QTParameterWidget ();

	virtual void UpdateParameterLevel (const ParameterLevel level, const QWidget * const parent_widget_p);


	void AddParameters (ParameterSet *params_p);

	json_t *GetParameterValuesAsJSON () const;

public slots:
	void ResetToDefaults ();

private slots:
	void OpenLink (const QString &link_r);

private:
	ParameterSet *qpw_params_p;

	const PrefsWidget * const qpw_prefs_widget_p;

	QHash <Parameter *, BaseParamWidget *> qpw_widgets_map;

	QFormLayout *qpw_form_layout_p;

	ParameterLevel qpw_level;

	QList <QWebView *> qpw_browsers;


	/**
	 * Create widget for parameter.
	 */
	BaseParamWidget *CreateWidgetForParameter (Parameter * const param_p);


	/**
	 * Get pre-existing widget for parameter.
	 */
	BaseParamWidget *GetWidgetForParameter (Parameter * const param_p);

};


#endif		/* #ifndef QT_PARAMETER_WIDGET_H */
