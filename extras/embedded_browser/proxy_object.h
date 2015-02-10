#ifndef PROXY_OBJECT_H
#define PROXY_OBJECT_H

#include <QObject>

#include "browser_widget.h"

/**
 * A class allowing to use for signals and slots
 * or objects that can't use them by default e.g.
 * c-based structures.
 *
 * @brief The Proxy class
 */
class ProxyObject : public QObject
{
	Q_OBJECT

signals:
	void TextChanged (const char *value_s);
	bool AccessTokenSet ();

public slots:
	void ChangeText (const char *value_s);
	void SetAccessToken (QByteArray ba);
	const QByteArray &GetAccessToken () const;
	void FindElements (BrowserWidget *w_p);

public:
	ProxyObject ();
	~ProxyObject ();

	void SetText (const char *value_s);
	const char *GetText () const;

	void SetSelector (const char *query_s);

private:
	char *po_text_s;
	const char *po_query_s;
	QByteArray po_access_token;
};


#endif		/* #ifndef PROXY_OBJECT_H */
