#include <iostream>

#include <QLayout>
#include <QGroupBox>
#include <QAction>
#include <QCheckBox>
#include <QMenuBar>
#include <QMenu>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QString>
#include <QTabWidget>

#include "prefs_widget.h"

#include "file_chooser_widget.h"
#include "qt_parameter_widget.h"


using namespace std;



PrefsWidget :: PrefsWidget (QWidget *parent_p,  ParameterLevel initial_level)
    :	QWidget (parent_p),
		pw_level (initial_level)
{
	pw_tabs_p = new QTabWidget;

	QVBoxLayout *layout_p = new QVBoxLayout;
	layout_p -> addWidget (pw_tabs_p);

	QHBoxLayout *buttons_layout_p = new QHBoxLayout;
	QPushButton *ok_button_p = new QPushButton (tr ("Ok"), this);
	QPushButton *cancel_button_p = new QPushButton (tr ("Cancel"), this);

	buttons_layout_p -> addWidget (ok_button_p);
	buttons_layout_p -> addWidget (cancel_button_p);

	layout_p -> addItem (buttons_layout_p);

	setLayout (layout_p);

	connect (ok_button_p, &QAbstractButton :: clicked, 	this, &PrefsWidget :: Accept);
	connect (cancel_button_p, &QAbstractButton :: clicked, 	this, &PrefsWidget :: Reject);
}


PrefsWidget :: ~PrefsWidget ()
{
}


void PrefsWidget :: Accept ()
{
	emit Finished (QDialog :: Accepted);
}


void PrefsWidget :: Reject ()
{
	emit Finished (QDialog :: Rejected);
}


void PrefsWidget :: AddServicePage (const json_t * const service_json_p)
{
	const char *service_name_s = GetServiceNameFromJSON (service_json_p);

	if (service_name_s)
		{
			const char *service_description_s = GetServiceNameFromJSON (service_json_p);

			if (service_description_s)
				{
					ParameterSet *params_p = CreateParameterSetFromJSON (service_json_p);

					if (params_p)
						{
							AddServicePage (service_name_s, service_description_s, params_p);
						}		/* if (params_p) */

				}		/* if (service_description_s) */

		}		/* if (service_name_s) */
}


void PrefsWidget :: AddServicePage (const char * const service_name_s, const char * const service_description_s, ParameterSet *params_p)
{
	QWidget *page_p = new QWidget;
	QLayout *layout_p = new QVBoxLayout;
	QTParameterWidget *widget_p = new QTParameterWidget (service_name_s, service_description_s, params_p, NULL, PL_BASIC);

	layout_p -> addWidget (widget_p);

	QString s ("Run ");
	s.append (service_name_s);
	QCheckBox *run_service_button_p = new QCheckBox (s, page_p);
	QPushButton *reset_button_p = new QPushButton ("Restore Defaults", page_p);

	QHBoxLayout *buttons_layout_p = new QHBoxLayout;
	buttons_layout_p -> addWidget (run_service_button_p);
	buttons_layout_p -> addWidget (reset_button_p);

	layout_p -> addItem (buttons_layout_p);

	page_p -> setLayout (layout_p);

	pw_tabs_p -> addTab (page_p, QString (service_name_s));
}



ParameterLevel PrefsWidget :: GetCurrentParameterLevel () const
{
	return pw_level;
}


void PrefsWidget :: SetInterfaceLevel (ParameterLevel level)
{
	if (pw_level != level)
		{
			emit InterfaceLevelChanged (level);
			pw_level = level;
		}		/* if (pw_level != level) */
}



void PrefsWidget :: ShowServiceConfigurationWidget (Service *service_p)
{
	if (service_p)
		{
			const char * const name_s = service_p -> se_get_service_name_fn ();
			const char * const description_s = service_p -> se_get_service_description_fn ();
			ParameterSet *params_p = service_p -> se_get_params_fn (service_p -> se_data_p);

			if (params_p)
				{
					QTParameterWidget *widget_p = new QTParameterWidget (name_s, description_s, params_p, this, pw_level);

					if (widget_p)
						{
							layout()->addWidget (widget_p);

						}		/* if (widget_p) */

					FreeParameterSet (params_p);
				}		/* if (params_p) */

		}		/* if (service_p) */

}

