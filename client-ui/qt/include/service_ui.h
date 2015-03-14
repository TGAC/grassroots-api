#ifndef SERVICE_UI_H
#define SERVICE_UI_H

#include "service_prefs_widget.h"

class ServiceUI
{
public:
  virtual void AddService (const char * const service_name_s, ServicePrefsWidget *service_widget_p) = 0;
  virtual QWidget *GetWidget () = 0;
};

#endif // SERVICE_UI_H

