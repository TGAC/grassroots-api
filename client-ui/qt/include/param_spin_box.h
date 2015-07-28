#ifndef PARAMETER_SPIN_BOX_H
#define PARAMETER_SPIN_BOX_H


#include <QWidget>
#include <QSpinBox>

#include "parameter.h"
#include "base_param_widget.h"



class ParamSpinBox : public BaseParamWidget
{
	Q_OBJECT


public:
	ParamSpinBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, bool signed_flag, QWidget *parent_p = 0);
	virtual ~ParamSpinBox ();


	virtual bool SetValueFromText (const char *value_s);

	virtual void SetDefaultValue ();
	virtual bool StoreParameterValue ();

protected:
	virtual QWidget *GetQWidget ();


private:
	QSpinBox *psb_spin_box_p;
	bool psb_signed_flag;
};


#endif		/* #ifndef PARAMETER_SPIN_BOX_H */
