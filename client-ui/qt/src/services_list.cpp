#include <QColor>

#include "services_list.h"

#include "filesystem_utils.h"

ServicesList :: ServicesList (QWidget *parent_p)
: QListWidget (parent_p)
{
  QHBoxLayout *layout_p = new QHBoxLayout;

  sl_stacked_widgets_p = new QStackedWidget (this);
  sl_services_p = new QListWidget (this);
 // sl_services_p -> setSelectionMode (QAbstractItemView :: SingleSelection);
 // sl_services_p -> setSelectionRectVisible (true);

  connect (sl_services_p, &QAbstractItemView :: clicked, this, &ServicesList :: SetCurrentService);
  connect (sl_services_p, &QAbstractItemView :: doubleClicked, this, &ServicesList :: ToggleServiceRunStatus);
  connect (sl_services_p, &QListWidget :: itemChanged, this, &ServicesList :: CheckServiceRunStatus);

  layout_p -> addWidget (sl_services_p);
  layout_p -> addWidget (sl_stacked_widgets_p);

  setLayout (layout_p);
}


void ServicesList :: SetCurrentService (const QModelIndex &index_r)
{
  const int row = index_r.row ();
  sl_stacked_widgets_p -> setCurrentIndex (row);
}


void ServicesList :: ToggleServiceRunStatus (const QModelIndex &index_r)
{
  int row = index_r.row ();
  ServicePrefsWidget *widget_p = static_cast <ServicePrefsWidget *> (sl_stacked_widgets_p -> widget (row));
  QListWidgetItem *list_item_p = sl_services_p -> item (row);

  if (list_item_p -> checkState() == Qt :: Checked)
    {
      widget_p -> SetRunFlag (false);
      list_item_p -> setCheckState (Qt :: Unchecked);
    }
  else
    {
      widget_p -> SetRunFlag (true);
      list_item_p -> setCheckState (Qt :: Unchecked);
    }
}


void ServicesList :: CheckServiceRunStatus (const QListWidgetItem *item_p)
{
  int row = sl_services_p -> row (item_p);
  bool state = item_p -> checkState () == Qt :: Checked;

  ServicePrefsWidget *widget_p = static_cast <ServicePrefsWidget *> (sl_stacked_widgets_p -> widget (row));
  widget_p -> SetRunFlag (state);

}


void ServicesList :: AddService (const char * const service_name_s, ServicePrefsWidget *services_widget_p)
{
	char * const icon_path_s = MakeFilename ("images", service_name_s);
	QString service_name (service_name_s);
	QListWidgetItem *item_p = 0;

	if (icon_path_s)
		{
			QIcon icon (icon_path_s);

			item_p = new QListWidgetItem (icon, service_name, sl_services_p);
		}
	else
		{
			item_p = new QListWidgetItem (service_name, sl_services_p);
		}

	sl_services_p -> addItem (item_p);
	sl_stacked_widgets_p -> addWidget (services_widget_p);

	item_p -> setCheckState (Qt :: Unchecked);
	item_p -> setFlags (Qt :: ItemIsEnabled | Qt :: ItemIsUserCheckable | Qt :: ItemIsSelectable);
}


QWidget *ServicesList :: GetWidget ()
{
	return this;
}
