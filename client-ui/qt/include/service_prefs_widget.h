#ifndef SERVICE_PREFS_WIDGET_H
#define SERVICE_PREFS_WIDGET_H

#include <QWidget>

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

	json_t *GetServiceParamsAsJSON () const;

signals:

public slots:
	void SetRunFlag (int state);

private:
	bool spw_run_flag;
	QTParameterWidget *spw_params_widget_p;
	const char *spw_service_name_s;
};

#endif // SERVICE_PREFS_WIDGET_H
