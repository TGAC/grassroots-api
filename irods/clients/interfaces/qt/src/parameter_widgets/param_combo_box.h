#ifndef PARAMETER_COMBO_BOX_H
#define PARAMETER_COMBO_BOX_H


#include <QWidget>
#include <QComboBox>

#include "parameter.h"
#include "base_param_widget.h"
#include "parameter_library_api.h"


class WHEATIS_CLIENT_QT_LIBRARY_API ParamComboBox : public BaseParamWidget
{
	Q_OBJECT


private slots:
	bool UpdateConfig (int state);

public:
	ParamComboBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p = 0);
	virtual ~ParamComboBox ();

	virtual QWidget *GetQWidget ();

private:
	QComboBox *pcb_combo_box_p;
};


#endif		/* #ifndef PARAMETER_COMBO_BOX_H */
