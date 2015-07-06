#include <QDebug>

#include "param_text_box.h"
#include "prefs_widget.h"

#include "string_utils.h"


ParamTextBox :: ParamTextBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
:		BaseParamWidget (param_p, options_widget_p)
{
	ptb_text_box_p = new QTextEdit (parent_p);

	connect (ptb_text_box_p, &QTextEdit :: textChanged, this, &ParamTextBox :: UpdateConfig);
}


ParamTextBox ::	~ParamTextBox ()
{
	delete ptb_text_box_p;
	ptb_text_box_p = NULL;
}


void ParamTextBox :: RemoveConnection ()
{
	disconnect (ptb_text_box_p, &QTextEdit :: textChanged, this, &ParamTextBox :: UpdateConfig);
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


bool ParamTextBox :: UpdateConfig (void)
{
	QString s = ptb_text_box_p -> toPlainText ();
	QByteArray ba = s.toLocal8Bit ();
	const char *value_s = ba.constData ();

	return UpdateConfigValue (value_s);
}


bool ParamTextBox :: UpdateConfigValue (const char * const value_s)
{
	bool success_flag = SetParameterValue (bpw_param_p, value_s);

	qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value_s;

	return success_flag;
}



bool ParamTextBox :: SetValueFromText (const char *value_s)
{
	QString s = ptb_text_box_p -> toPlainText ();
	qDebug () << "old " << s;

	ptb_text_box_p -> setPlainText (value_s);

	s = ptb_text_box_p -> toPlainText ();
	qDebug () << "new " << s;

	return true;
}
