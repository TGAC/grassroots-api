#ifndef BROWSER_WIDGET_H
#define BROWSER_WIDGET_H

#include <QNetworkReply>
#include <QWidget>
#include <QWebView>
#include <QUrl>
#include <QWebElementCollection>


class BrowserWidget : public QWidget
{
	Q_OBJECT

public:
	BrowserWidget(QWidget *parent = 0);
	~BrowserWidget();

	void SetUrl (char *url_s);

	QWebElementCollection Find (const char * const query_s);


signals:
	void UrlChanged (QString &str_r);
	void TitleChanged (const char *title_s);
	void FinishedLoading (BrowserWidget *w_p);
	void FinishedResponse (BrowserWidget *w_p);

private slots:
	void ChangeTitle (const QString &title_r);
	void ChangeUrl (const QUrl &url_r);
	void  ReponseFinished (QNetworkReply *reply_p);
void  LoadingComplete (bool ok);

protected:
	QWebView *bw_browser_p;
	QNetworkAccessManager *bw_network_manager_p;
};

#endif // BROWSER_WIDGET_H
