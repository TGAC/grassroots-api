#include "text_viewer.h"

TextViewer :: TextViewer (QWidget *parent_p)
:	QTextEdit (parent_p)
{

}



const char *TextViewer :: GetText () const
{
	QString s = toPlainText ();
	QByteArray ba = s.toLocal8Bit ();
	const char *data_s = ba.constData ();

	return data_s;
}


QWidget *TextViewer :: GetWidget ()
{
	return this;
}
