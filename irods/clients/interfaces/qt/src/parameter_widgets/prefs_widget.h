#ifndef PREFS_WIDGET_H
#define PREFS_WIDGET_H

#include <QMainWindow>
#include <QTabWidget>
#include <QWidget>

#include "parameter_library_api.h"
//#include "library_prefs_widget.h"

#include "parameter.h"


// forward class declaration
class LibraryPrefsWidget;

/**
 * @brief The PrefsWidget class
 */
class WHEATIS_CLIENT_QT_LIBRARY_API PrefsWidget : public QWidget
{
	Q_OBJECT

signals:
	void InterfaceLevelChanged (ParameterLevel level);

public slots:
	void SetInterfaceLevel (ParameterLevel level);

public:

	/*********************/
	/***** FUNCTIONS *****/
	/*********************/

	explicit PrefsWidget (QWidget *parent_p, ParameterLevel initial_level);

	~PrefsWidget ();

	ParameterLevel GetCurrentParameterLevel () const;


private:

	/*********************/
	/***** VARIABLES *****/
	/*********************/

	ParameterLevel pw_level;
};


#endif // PREFS_WIDGET_H
