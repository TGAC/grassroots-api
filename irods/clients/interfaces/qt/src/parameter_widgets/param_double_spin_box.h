#ifndef PARAMETER_DOUBLE_SPIN_BOX_H
#define PARAMETER_DOUBLE_SPIN_BOX_H


#include <QWidget>
#include <QDoubleSpinBox>

#include "parameter.h"
#include "base_param_widget.h"

#include "parameter_library_api.h"



class WHEATIS_CLIENT_QT_LIBRARY_API ParamDoubleSpinBox : public BaseParamWidget
{
	Q_OBJECT

private slots:
	bool UpdateConfig (double value);

public:
	ParamDoubleSpinBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p = 0);
	virtual ~ParamDoubleSpinBox ();

	virtual QWidget *GetQWidget ();

private:
	QDoubleSpinBox *pdsb_spinner_p;
};


#endif		/* #ifndef PARAMETER_DOUBLE_SPIN_BOX_H */
