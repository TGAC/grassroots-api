#include <QAction>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMimeData>
#include <QPluginLoader>
#include <QUrl>
#include <QWidget>

#include "prefs_widget.h"
#include "prefs_window.h"



PrefsWindow :: PrefsWindow (ParameterSet *param_set_p)
{
	pw_prefs_widget_p = new PrefsWidget (this, PL_BASIC);


	QHBoxLayout *layout_p = new QHBoxLayout;
	QWidget *w_p = new QWidget;

	GenerateMenu ();

	layout_p -> addWidget (pw_prefs_widget_p);

//	QWidget *plugin_widget_p = LoadPluginWidget (config_p);
//	if (plugin_widget_p)
//		{
//			layout_p -> addWidget (plugin_widget_p);
//		}

	w_p -> setLayout (layout_p);

	setCentralWidget (w_p);

	setWindowTitle ("Options");
	setWindowIcon (QIcon ("../images/prefs.png"));
}


PrefsWindow :: ~PrefsWindow ()
{
}


void PrefsWindow :: GenerateMenu ()
{
	QMenuBar *menu_bar_p = menuBar ();

	// File Menu
	QMenu *menu_p = menu_bar_p -> addMenu (tr ("&File"));

	// Tools Menu
	menu_p = menu_bar_p -> addMenu (tr ("&Tools"));

	QMenu *sub_menu_p = new QMenu (tr ("Level"));
	QActionGroup *interface_levels_p = new QActionGroup (this);

	// Easy Level
	QAction *action_p = new QAction (tr ("Basic"), this);
	action_p -> setStatusTip (tr ("Basic"));
	action_p -> setCheckable (true);
	action_p -> setChecked (true);
	connect (action_p, &QAction :: triggered, this, &PrefsWindow :: SetBasicInterfaceLevel);
	sub_menu_p -> addAction (action_p);
	interface_levels_p -> addAction (action_p);

	// Intermediate Level
	action_p = new QAction (tr ("Intermediate"), this);
	action_p -> setStatusTip (tr ("Intermediate"));
	action_p -> setCheckable (true);
	connect (action_p, &QAction :: triggered, this, &PrefsWindow :: SetIntermediateInterfaceLevel);
	sub_menu_p -> addAction (action_p);
	interface_levels_p -> addAction (action_p);

	// Advanced Lavel
	action_p = new QAction (tr ("Advanced"), this);
	action_p -> setStatusTip (tr ("Advanced"));
	action_p -> setCheckable (true);
	connect (action_p, &QAction :: triggered, this, &PrefsWindow :: SetAdvancedInterfaceLevel);
	sub_menu_p -> addAction (action_p);
	interface_levels_p -> addAction (action_p);


	menu_p -> addMenu (sub_menu_p);

}


void PrefsWindow :: SetBasicInterfaceLevel ()
{
	pw_prefs_widget_p -> SetInterfaceLevel (PL_BASIC);
}


void PrefsWindow :: SetIntermediateInterfaceLevel ()
{
	pw_prefs_widget_p -> SetInterfaceLevel (PL_INTERMEDIATE);
}


void PrefsWindow :: SetAdvancedInterfaceLevel ()
{
	pw_prefs_widget_p -> SetInterfaceLevel (PL_ADVANCED);
}
