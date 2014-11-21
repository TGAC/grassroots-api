#include <QHBoxLayout>
#include <QtNetwork>
#include "browser_widget.h"

BrowserWidget::BrowserWidget(QWidget *parent)
	: QWidget(parent)
{
	bw_browser_p = new QWebView;
	QHBoxLayout *layout_p = new QHBoxLayout;

	layout_p -> addWidget (bw_browser_p);

	connect (bw_browser_p, &QWebView :: urlChanged, this, &BrowserWidget :: ChangeUrl);

	connect (bw_browser_p, &QWebView :: titleChanged, this, &BrowserWidget :: ChangeTitle);


	setLayout (layout_p);
}


BrowserWidget::~BrowserWidget()
{

}


void BrowserWidget  :: SetUrl (char *url_s)
{
	QUrl url (url_s);
	bw_browser_p -> setUrl (url);
}


void  BrowserWidget :: ChangeUrl (const QUrl &url_r)
{
	QString s = url_r.toString ();
	QByteArray ba = s.toLocal8Bit ();
	const char *url_s = ba.constData ();

	emit UrlChanged (s);
}


void  BrowserWidget :: ChangeTitle (const QString &title_r)
{
	QByteArray ba = title_r.toLocal8Bit ();
	const char *title_s = ba.constData ();
	const char * const google_code_prefix_s = "Success code=";


	if (title_r.startsWith (google_code_prefix_s))
		{
			emit TitleChanged (title_s + strlen (google_code_prefix_s));
			close ();
		}
}
