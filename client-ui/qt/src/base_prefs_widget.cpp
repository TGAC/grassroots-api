#include <QDebug>
#include <QVBoxLayout>

#include "base_prefs_widget.h"


#include "string_utils.h"



BasePrefsWidget :: BasePrefsWidget (const QString &title_r, const PrefsWidget * const prefs_widget_p)
	:	bpw_prefs_widget_p (prefs_widget_p)
{
	QVBoxLayout *layout_p = new QVBoxLayout;


	connect (bpw_prefs_widget_p, &PrefsWidget :: InterfaceLevelChanged, this, &BasePrefsWidget :: CheckInterfaceLevel);

	setLayout (layout_p);

	QByteArray ba = title_r.toLocal8Bit ();
	bpw_name_s = CopyToNewString (ba.constData (), 0, false);
}


BasePrefsWidget :: ~BasePrefsWidget ()
{
	if (bpw_name_s)
		{
			FreeCopiedString (bpw_name_s);
		}
}

