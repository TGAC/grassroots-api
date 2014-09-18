#ifndef PARAMETER_TEXT_BOX_H
#define PARAMETER_TEXT_BOX_H


#include <QWidget>
#include <QLineEdit>

#include "parameter.h"

#include "base_param_widget.h"



class ParamTextBox : public BaseParamWidget
{
	Q_OBJECT


private slots:
	bool UpdateConfig (const QString &value_r);

public:
	ParamTextBox (const Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p = 0);
	virtual ~ParamTextBox ();

	virtual QWidget *GetQWidget ();

	virtual void RemoveConnection ();

protected:
	QLineEdit *ptb_text_box_p;
};


#endif		/* #ifndef PARAMETER_TEXT_BOX_H */
