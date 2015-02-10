#include <QHBoxLayout>
#include <QtNetwork>
#include <QWebFrame>

#include "browser_widget.h"

BrowserWidget::BrowserWidget(QWidget *parent)
	: QWidget(parent)
{
	bw_browser_p = new QWebView;

	QHBoxLayout *layout_p = new QHBoxLayout;

	layout_p -> addWidget (bw_browser_p);

	connect (bw_browser_p, &QWebView :: urlChanged, this, &BrowserWidget :: ChangeUrl);

	connect (bw_browser_p, &QWebView :: titleChanged, this, &BrowserWidget :: ChangeTitle);

	connect (bw_browser_p, &QWebView :: loadFinished, this, &BrowserWidget :: LoadingComplete);

	bw_network_manager_p = new QNetworkAccessManager (parent);
	connect  (bw_network_manager_p, &QNetworkAccessManager :: finished, this, &BrowserWidget :: ReponseFinished);



	setLayout (layout_p);
}


BrowserWidget::~BrowserWidget()
{
	delete bw_browser_p;
	delete bw_network_manager_p;
}


void BrowserWidget  :: SetUrl (char *url_s)
{
	QUrl url (url_s);
	QNetworkRequest req (url);

	bw_network_manager_p -> get (req);
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


QWebElementCollection BrowserWidget :: Find (const char * const query_s)
{
	QWebFrame *frame_p = bw_browser_p -> page () -> mainFrame ();
	QWebElementCollection collection = frame_p -> findAllElements (query_s);

	return collection;
}



void  BrowserWidget :: LoadingComplete (bool ok)
{
	emit FinishedLoading (this);
}

void  BrowserWidget :: ReponseFinished (QNetworkReply *reply_p)
{
	QByteArray response = reply_p -> readAll ();
	QString html (response);
	QString auth_header ("Authorization");
	QByteArray ba = auth_header.toLocal8Bit ();

	if (reply_p -> hasRawHeader (ba ))
		{
			QByteArray header_value = reply_p -> rawHeader ("Authorization: Bearer");
		}

	bw_browser_p -> setHtml (html, reply_p -> request ().url ());

	emit FinishedResponse (this);
}
