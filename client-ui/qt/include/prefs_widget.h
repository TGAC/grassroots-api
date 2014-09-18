#ifndef EPRMAIN_H
#define EPRMAIN_H

#include <QMainWindow>
#include <QTabWidget>
#include <QWidget>

#include "parameter.h"
#include "service.h"

// forward class declaration
class LibraryPrefsWidget;

/**
 * @brief The PrefsWidget class
 */
class PrefsWidget : public QWidget
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


	void ShowServiceConfigurationWidget (Service *service_p);

	ParameterLevel GetCurrentParameterLevel () const;


	void AddServicePage (QTabWidget *tab_p, QString title_r, const char * const plugin_s);

private:

	/*********************/
	/***** VARIABLES *****/
	/*********************/

	ParameterLevel pw_level;



};


#endif // EPRMAIN_H
