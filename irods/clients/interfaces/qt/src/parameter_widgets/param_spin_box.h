#ifndef PARAMETER_SPIN_BOX_H
#define PARAMETER_SPIN_BOX_H


#include <QWidget>
#include <QSpinBox>

#include "parameter.h"
#include "base_param_widget.h"
#include "parameter_library_api.h"


class WHEATIS_CLIENT_QT_LIBRARY_API ParamSpinBox : public BaseParamWidget
{
	Q_OBJECT

private slots:
	bool UpdateConfig (int value);

public:
	ParamSpinBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p = 0);
	virtual ~ParamSpinBox ();

	virtual QWidget *GetQWidget ();

private:
	QSpinBox *psb_spin_box_p;

};


#endif		/* #ifndef PARAMETER_SPIN_BOX_H */
