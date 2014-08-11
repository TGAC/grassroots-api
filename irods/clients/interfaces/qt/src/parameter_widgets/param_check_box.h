#ifndef PARAMETER_CHECK_BOX_H
#define PARAMETER_CHECK_BOX_H


#include <QWidget>
#include <QCheckBox>

#include "parameter.h"
#include "base_param_widget.h"

#include "parameter_library_api.h"

class WHEATIS_CLIENT_QT_LIBRARY_API ParamCheckBox : public BaseParamWidget
{
	Q_OBJECT

private slots:
	bool UpdateConfig (int state);

public:
	ParamCheckBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p = 0);
	virtual ~ParamCheckBox ();

	virtual QWidget *GetQWidget ();

private:
	QCheckBox *pcb_check_box_p;

};


#endif		/* #ifndef PARAMETER_CHECK_BOX_H */
