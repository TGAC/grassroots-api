#include <QDebug>

#include "param_text_box.h"
#include "prefs_widget.h"

#include "string_utils.h"


ParamTextBox :: ParamTextBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QLineEdit :: EchoMode echo, QWidget *parent_p)
:		BaseParamWidget (param_p, options_widget_p)
{
	ptb_text_box_p = new QLineEdit (parent_p);
	ptb_text_box_p -> setEchoMode (echo);

	connect (ptb_text_box_p, &QLineEdit :: textChanged, this, &ParamTextBox :: UpdateConfig);
}


ParamTextBox ::	~ParamTextBox ()
{
	delete ptb_text_box_p;
	ptb_text_box_p = NULL;
}


void ParamTextBox :: RemoveConnection ()
{
	disconnect (ptb_text_box_p, &QLineEdit :: textChanged, this, &ParamTextBox :: UpdateConfig);
}


void ParamTextBox :: SetDefaultValue ()
{
	const char *value_s = bpw_param_p -> pa_default.st_string_value_s;

	ptb_text_box_p -> setText (value_s);
}


QWidget *ParamTextBox :: GetQWidget ()
{
	return ptb_text_box_p;
}


bool ParamTextBox :: UpdateConfig (const QString &value_r)
{
	QByteArray ba = value_r.toLocal8Bit ();
	const char *value_s = ba.constData ();
	bool success_flag = SetParameterValue (bpw_param_p, value_s);

	qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value_s;

	return success_flag;
}

