#include <QGroupBox>

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

  QGroupBox *box_p = new QGroupBox (tr ("Services1"));
  QVBoxLayout *services_layout_p = new QVBoxLayout;

  services_layout_p -> addWidget (sl_services_p);
  box_p -> setLayout (services_layout_p);
  box_p -> setAlignment (Qt :: AlignRight);

  layout_p -> addWidget (box_p);
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
  QListWidgetItem *list_item_p = sl_services_p -> item (row);

  CheckServiceRunStatus (list_item_p);
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
	ServicesListItem *item_p = 0;

	if (icon_path_s)
		{
			QIcon icon (icon_path_s);

			item_p = new ServicesListItem (icon, service_name, sl_services_p);
		}
	else
		{
			item_p = new ServicesListItem (service_name, sl_services_p);
		}

	sl_services_p -> addItem (item_p);
	int index = sl_stacked_widgets_p -> addWidget (services_widget_p);

	item_p -> setCheckState (Qt :: Unchecked);
	item_p -> setFlags (Qt :: ItemIsEnabled | Qt :: ItemIsUserCheckable | Qt :: ItemIsSelectable);

	connect (services_widget_p, &ServicePrefsWidget :: RunStatusChanged, this, &ServicesList :: SetServiceRunStatus);
}


QWidget *ServicesList :: GetWidget ()
{
	return this;
}


void ServicesList :: SetServiceRunStatus (const char * const service_name_s, bool state)
{
	QString service_name (service_name_s);

	QList <QListWidgetItem *> widgets = sl_services_p -> findItems (service_name, Qt :: MatchExactly);
	if (widgets.size () == 1)
		{
			QListWidgetItem *item_p = widgets.at (0);

			item_p -> setCheckState (state ? Qt :: Checked : Qt :: Unchecked);
		}
}


ServicesListItem :: ServicesListItem (const QIcon &icon_r, const QString &service_name_r, QListWidget *list_p)
: QListWidgetItem (icon_r, service_name_r, list_p)
{}


ServicesListItem :: ServicesListItem (const QString &service_name_r, QListWidget *list_p)
: QListWidgetItem (service_name_r, list_p)
{}


/*
void ServicesListItem :: SetRunStatus (bool state)
{
	setCheckState (state ? Qt :: Checked : Qt :: Unchecked);
}
*/