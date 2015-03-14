#ifndef PARAMETER_COMBO_BOX_H
#define PARAMETER_COMBO_BOX_H


#include <QWidget>
#include <QComboBox>
#include <QGroupBox>

#include "parameter.h"
#include "base_param_widget.h"



class ParamComboBox : public BaseParamWidget
{
	Q_OBJECT

private slots:
	bool UpdateConfig (int state);

public:
	ParamComboBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p = 0);
	virtual ~ParamComboBox ();

	virtual QWidget *GetQWidget ();


	virtual void SetDefaultValue ();

	virtual bool SetValueFromText (const char *value_s);

private:
	QComboBox *pcb_combo_box_p;
	QGroupBox *pcb_group_p;
};


#endif		/* #ifndef PARAMETER_COMBO_BOX_H */
