#ifndef PREFS_WINDOW_H
#define PREFS_WINDOW_H


#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMainWindow>

#include "prefs_widget.h"
#include "parameter_library_api.h"

#include "parameter.h"

class WHEATIS_CLIENT_QT_LIBRARY_API PrefsWindow : public QMainWindow
{
	Q_OBJECT

public slots:
	void SetBasicInterfaceLevel ();
	void SetIntermediateInterfaceLevel ();
	void SetAdvancedInterfaceLevel ();


public:
	PrefsWindow (ParameterSet *param_set_p);
	~PrefsWindow ();


private:
	PrefsWidget *pw_prefs_widget_p;

	void GenerateMenu ();

};


#endif		/* #ifndef EPR_WINDOW_H */
