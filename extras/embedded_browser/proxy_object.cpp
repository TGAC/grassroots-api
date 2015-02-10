#include <QWebElementCollection>
#include <QWebElement>

#include "proxy_object.h"


ProxyObject :: ProxyObject ()
{
	po_text_s = NULL;
	po_query_s  = NULL;
}


ProxyObject :: ~ProxyObject ()
{
	if (po_text_s)
		{
			free (po_text_s);
		}
}


void ProxyObject :: SetAccessToken (QByteArray ba)
{
	po_access_token = ba;
	emit AccessTokenSet ();
}


const QByteArray &ProxyObject :: GetAccessToken () const
{
	return po_access_token;
}


void ProxyObject :: SetSelector (const char  *query_s)
{
	po_query_s = query_s;
}


void ProxyObject :: FindElements (BrowserWidget *w_p)
{
	QWebElementCollection c = w_p -> Find (po_query_s);

	int size = c.count();

	for (int i = 0; i < size; ++ i)
		{
			QWebElement el = c.at (i);

			if (el.hasAttribute ("href"))
				{
					QString s  = el.attribute ("href");
				QString title = el.toInnerXml().trimmed().replace (QRegExp ("\\s+"), " ");

					qDebug () << s.trimmed() << " -> " << title;
				}
		}

}


void ProxyObject :: SetText (const char *value_s)
{
	bool emit_flag = true;

	if (po_text_s)
		{
			if (value_s)
				{
					if (strcmp (po_text_s, value_s) != 0)
						{
							char *temp_s = strdup (value_s);

							if (temp_s)
								{
									free (po_text_s);
									po_text_s = temp_s;
								}
						}
					else
						{
							emit_flag = false;
						}
				}
			else
				{
					free (po_text_s);
					po_text_s = NULL;
				}
		}
	else
		{
			if (value_s)
				{
					char *temp_s = strdup (value_s);

					if (temp_s)
						{
							po_text_s = temp_s;
						}
				}
			else
				{
					emit_flag = false;
				}
		}


	if (emit_flag)
		{
			emit TextChanged (po_text_s);
		}
}


void ProxyObject :: ChangeText (const char *value_s)
{
	SetText (value_s);
}


const char *ProxyObject :: GetText () const
{
	return po_text_s;
}


