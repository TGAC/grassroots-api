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
	void Finished (int result);

public slots:
	void SetInterfaceLevel (ParameterLevel level);
	void Accept ();
	void Reject ();

public:

	/*********************/
	/***** FUNCTIONS *****/
	/*********************/

	explicit PrefsWidget (QWidget *parent_p, ParameterLevel initial_level);

	~PrefsWidget ();


	void ShowServiceConfigurationWidget (Service *service_p);

	ParameterLevel GetCurrentParameterLevel () const;


	void AddServicePage (const json_t * const service_json_p);
	void AddServicePage (const char * const service_name_s, const char * const service_description_s, ParameterSet *params_p);


private:

	/*********************/
	/***** VARIABLES *****/
	/*********************/

	ParameterLevel pw_level;
	QTabWidget *pw_tabs_p;


};


#endif // EPRMAIN_H
