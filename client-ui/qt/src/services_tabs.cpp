#include "services_tabs.h"

#include "filesystem_utils.h"
#include "string_utils.h"


ServicesTabs :: ServicesTabs (QWidget *parent_p)
: QTabWidget (parent_p)
{

}


void ServicesTabs :: AddService (const char * const service_name_s, ServicePrefsWidget *service_widget_p)
{
	char * const icon_path_s = MakeFilename ("images", service_name_s);
	QString service_name (service_name_s);

	if (icon_path_s)
		{
			QIcon icon (icon_path_s);

			addTab (service_widget_p, icon, service_name);

			FreeCopiedString (icon_path_s);
		}
	else
		{
			addTab (service_widget_p, service_name);
		}
}


QWidget *ServicesTabs :: GetWidget ()
{
	return this;
}


QWidget *ServicesTabs :: GetServiceWidget (const char * const service_name_s)
{
	QWidget *widget_p = 0;
	QString name (service_name_s);

	for (int i = sl_services_p -> count (); i >= 0; -- i)
		{
			const QListWidgetItem *item_p = sl_services_p -> item (i);
			const QString s = item_p -> text ();

			if (s.compare (name) == 0)
				{
					widget_p = sl_stacked_widgets_p -> widget (i);
					i = -1;		/* force exit from loop */
				}
		}

	return widget_p;
}
