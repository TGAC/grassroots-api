#ifndef PREFS_WIDGET_H
#define PREFS_WIDGET_H

#include <QList>
#include <QMainWindow>
#include <QStackedWidget>
#include <QTabWidget>
#include <QWidget>

#include "parameter.h"

#include "jansson.h"

#include "service_prefs_widget.h"
#include "services_list.h"


/**
 * @brief The PrefsWidget class
 */
class PrefsWidget : public QWidget
{
	Q_OBJECT

signals:
	void InterfaceLevelChanged (ParameterLevel level);
	void RunServices (bool run_flag);

public slots:
	void SetInterfaceLevel (ParameterLevel level);
	void Accept ();
	void Reject ();

public:

	/*********************/
	/***** FUNCTIONS *****/
	/*********************/

	explicit PrefsWidget (QWidget *parent_p, ParameterLevel initial_level, bool tabbed_display_flag);

	~PrefsWidget ();

	ParameterLevel GetCurrentParameterLevel () const;


	void CreateAndAddServicePage (const json_t * const service_json_p);
	void CreateAndAddServicePage (const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s , ParameterSet *params_p);


	json_t *GetUserValuesAsJSON (bool full_flag) const;

private:

	/*********************/
	/***** VARIABLES *****/
	/*********************/

	ParameterLevel pw_level;
	ServiceUI *pw_services_ui_p;
	QList <ServicePrefsWidget *> pw_service_widgets;
};


#endif // PREFS_WIDGET_H
