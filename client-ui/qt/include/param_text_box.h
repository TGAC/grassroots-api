#ifndef PARAM_TEXT_BOX_H
#define PARAM_TEXT_BOX_H

#include <QWidget>
#include <QTextEdit>

#include "parameter.h"

#include "base_param_widget.h"



class ParamTextBox : public BaseParamWidget
{
	Q_OBJECT


private slots:
	bool UpdateConfig ();
	bool UpdateConfigValue (const char * const value_s);

public:
	ParamTextBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p = 0);
	virtual ~ParamTextBox ();

	virtual void RemoveConnection ();

	virtual void SetDefaultValue ();

	virtual bool SetValueFromText (const char *value_s);

protected:
	QTextEdit *ptb_text_box_p;

	virtual QWidget *GetQWidget ();


};

#endif // PARAM_TEXT_BOX_H
