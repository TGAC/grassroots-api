#ifndef SERVICES_LIST_H
#define SERVICES_LIST_H

#include <QListWidget>
#include <QStackedWidget>
#include <QStyledItemDelegate>

#include "service_ui.h"

class ServicesList : public QListWidget, public ServiceUI
{
  Q_OBJECT

public:
  ServicesList (QWidget *parent_p = 0);

  virtual void AddService (const char * const service_name_s, ServicePrefsWidget *services_widget_p);
  virtual QWidget *GetWidget ();

private slots:
  void ToggleServiceRunStatus (const QModelIndex &index_r);
  void CheckServiceRunStatus (const QListWidgetItem *item_p);
  void SetCurrentService (const QModelIndex &index_r);
  void SetServiceRunStatus (const char * const service_name_s, bool state);

private:
  QStackedWidget *sl_stacked_widgets_p;
  QListWidget *sl_services_p;
};



class ServicesListItem : public QListWidgetItem, public QObject
{


public:
  ServicesListItem (const QIcon &icon_r, const QString &service_name_r, QListWidget *list_p);
  ServicesListItem (const QString &service_name_r, QListWidget *list_p);

  /*
public slots:
  void SetRunStatus (bool state);
  */
};


#endif // SERVICES_LIST_H

