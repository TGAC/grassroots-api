#include <QDebug>
#include <QVBoxLayout>

#include "base_prefs_widget.h"


#ifdef _DEBUG
	#define BASE_PREFS_WIDGET_DEBUG (STM_LEVEL_FINE)
#else
	#define BASE_PREFS_WIDGET_DEBUG (STM_LEVEL_NONE)
#endif


BasePrefsWidget :: BasePrefsWidget (const QString &title_r, const PrefsWidget * const prefs_widget_p)
	:	bpw_prefs_widget_p (prefs_widget_p)
{
	QVBoxLayout *layout_p = new QVBoxLayout;

	connect (bpw_prefs_widget_p, &PrefsWidget :: InterfaceLevelChanged, this, &BasePrefsWidget :: CheckInterfaceLevel);

	setLayout (layout_p);

	QByteArray ba = title_r.toLocal8Bit ();
	bpw_name_s = strdup (ba.constData ());
}


BasePrefsWidget :: ~BasePrefsWidget ()
{
	if (bpw_name_s)
		{
			free (bpw_name_s);
		}
}

