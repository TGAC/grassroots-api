#include <QDebug>

#include "param_line_edit.h"
#include "prefs_widget.h"

#include "string_utils.h"


ParamLineEdit :: ParamLineEdit (Parameter * const param_p, const PrefsWidget * const options_widget_p, QLineEdit :: EchoMode echo, QWidget *parent_p)
:		BaseParamWidget (param_p, options_widget_p)
{
	ple_text_box_p = new QLineEdit (parent_p);
	ple_text_box_p -> setEchoMode (echo);

	connect (ple_text_box_p, &QLineEdit :: textChanged, this, &ParamLineEdit :: UpdateConfig);
}


ParamLineEdit ::	~ParamLineEdit ()
{
	delete ple_text_box_p;
	ple_text_box_p = NULL;
}


void ParamLineEdit :: RemoveConnection ()
{
	disconnect (ple_text_box_p, &QLineEdit :: textChanged, this, &ParamLineEdit :: UpdateConfig);
}


void ParamLineEdit :: SetDefaultValue ()
{
	const char *value_s = bpw_param_p -> pa_default.st_string_value_s;

	ple_text_box_p -> setText (value_s);
}


QWidget *ParamLineEdit :: GetQWidget ()
{
	return ple_text_box_p;
}


bool ParamLineEdit :: UpdateConfig (const QString &value_r)
{
	QByteArray ba = value_r.toLocal8Bit ();
	const char *value_s = ba.constData ();

	return UpdateConfigValue (value_s);
}


bool ParamLineEdit :: UpdateConfigValue (const char * const value_s)
{
	bool success_flag = SetParameterValue (bpw_param_p, value_s);

	qDebug () << "Setting " << bpw_param_p -> pa_name_s << " to " << value_s;

	return success_flag;
}



bool ParamLineEdit :: SetValueFromText (const char *value_s)
{
	QString s = ple_text_box_p -> text ();
	qDebug () << "old " << s;

	ple_text_box_p -> setText (value_s);

	s = ple_text_box_p -> text ();
	qDebug () << "new " << s;

	return true;
}
