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

