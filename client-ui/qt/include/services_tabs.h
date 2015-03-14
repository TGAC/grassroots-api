#ifndef SERVICES_TABS_H
#define SERVICES_TABS_H

#include <QTabWidget>

#include "service_ui.h"


class ServicesTabs : public QTabWidget, public ServiceUI
{
public:
  ServicesTabs (QWidget *parent_p = 0);

  virtual void AddService (const char * const service_name_s, ServicePrefsWidget *service_widget_p);
  virtual QWidget *GetWidget ();
  virtual QWidget *GetServiceWidget (const char * const service_name_s);

};

#endif // SERVICES_TABS_H

