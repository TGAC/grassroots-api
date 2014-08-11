#include <iostream>

#include <QLayout>
#include <QGroupBox>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QTabWidget>

#include "prefs_widget.h"


using namespace std;



PrefsWidget :: PrefsWidget (QWidget *parent_p, ParameterLevel initial_level)
    :	QWidget (parent_p),
		pw_level (initial_level)
{
	QTabWidget *tabs_p = new QTabWidget;



	QHBoxLayout *layout_p = new QHBoxLayout;
	layout_p -> addWidget (tabs_p);
	setLayout (layout_p);
}


PrefsWidget :: ~PrefsWidget ()
{
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


