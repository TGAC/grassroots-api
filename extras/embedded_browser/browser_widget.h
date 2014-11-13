#ifndef BROWSER_WIDGET_H
#define BROWSER_WIDGET_H

#include <QWidget>
#include <QWebView>
#include <QUrl>

class BrowserWidget : public QWidget
{
	Q_OBJECT

public:
	BrowserWidget(QWidget *parent = 0);
	~BrowserWidget();

	void SetUrl (char *url_s);

signals:
	void UrlChanged (QString &str_r);
	void TitleChanged (const char *title_s);


private slots:
	void ChangeTitle (const QString &title_r);
	void ChangeUrl (const QUrl &url_r);

protected:
	QWebView *bw_browser_p;
};

#endif // BROWSER_WIDGET_H
