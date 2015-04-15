#ifndef SERVICE_PREFS_WIDGET_H
#define SERVICE_PREFS_WIDGET_H

#include <QWidget>
#include <QCheckBox>

#include "parameter_set.h"
#include "qt_parameter_widget.h"


#include "jansson.h"

class ServicePrefsWidget : public QWidget
{
	Q_OBJECT
public:
	explicit ServicePrefsWidget (const char * const service_name_s, const char * const service_description_s, const char * const service_info_s, ParameterSet *params_p, QWidget *parent_p = 0);
	~ServicePrefsWidget ();

	bool GetRunFlag () const;

	json_t *GetServiceParamsAsJSON (bool full_flag) const;

	const char *GetServiceName () const;

	bool SetServiceParams (json_t *service_config_p);

	void CheckInterfaceLevel (ParameterLevel level);

signals:
	void RunStatusChanged (const char * const service_name_s, bool status);


public slots:
	void SetRunFlag (bool state);
	void ToggleRunFlag ();

private:
	QTParameterWidget *spw_params_widget_p;
	const char *spw_service_name_s;
	QCheckBox *spw_run_service_button_p;
};

#endif // SERVICE_PREFS_WIDGET_H
