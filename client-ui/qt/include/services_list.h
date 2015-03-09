#ifndef SERVICES_LIST_H
#define SERVICES_LIST_H

#include <QListWidget>
#include <QStackedWidget>

#include "service_ui.h"

class ServicesList : public QListWidget, public ServiceUI
{
  Q_OBJECT

public:
  ServicesList (QWidget *parent_p = 0);

  virtual void AddService (const char * const service_name_s, ServicePrefsWidget *services_widget_p);
  virtual QWidget *GetWidget ();

private slots:
  void SetCurrent (int row);
  void ToggleServiceRunStatus (const QModelIndex &index_r);

private:
  QStackedWidget *sl_stacked_widgets_p;
  QListWidget *sl_services_p;
};

#endif // SERVICES_LIST_H

