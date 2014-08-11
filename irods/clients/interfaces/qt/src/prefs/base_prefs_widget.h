#ifndef BASE_PREFS_WIDGET_H
#define BASE_PREFS_WIDGET_H

#include <QGroupBox>

#include "qt_parameter_widget.h"
#include "prefs_widget.h"
#include "parameter_library_api.h"
#include "linked_list.h"



class WHEATIS_CLIENT_QT_LIBRARY_API BasePrefsWidget : public QGroupBox
{
	Q_OBJECT

protected slots:
	virtual void CheckInterfaceLevel (ParameterLevel level) = 0;

public:
	BasePrefsWidget (const QString &title_r, const PrefsWidget * const prefs_widget_p);
	~BasePrefsWidget ();

protected:
	const PrefsWidget * const bpw_prefs_widget_p;
	char *bpw_name_s;
};

#endif		/* #ifndef BASE_PREFS_WIDGET_H */
