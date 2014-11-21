#ifndef PROXY_OBJECT_H
#define PROXY_OBJECT_H

#include <QObject>

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

public slots:
	void ChangeText (const char *value_s);

public:
	ProxyObject ();
	~ProxyObject ();

	void SetText (const char *value_s);
	const char *GetText () const;

private:
	char *po_text_s;
};


#endif		/* #ifndef PROXY_OBJECT_H */
