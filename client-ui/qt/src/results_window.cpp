#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "results_window.h"


ResultsWindow :: ResultsWindow (QMainWindow *parent_p)
{
	QVBoxLayout *layout_p = new QVBoxLayout;

	rw_results_p = new ResultsWidget;
	layout_p -> addWidget (rw_results_p);

	QGroupBox *buttons_p = new QGroupBox;
	QHBoxLayout *buttons_layout_p = new QHBoxLayout;

	QPushButton *btn_p = new QPushButton (QIcon ("images/ok"), tr ("Ok"), buttons_p);
	buttons_layout_p -> addWidget (btn_p);
	connect (btn_p, &QPushButton :: clicked, this, &ResultsWindow :: close);

	btn_p = new QPushButton (QIcon ("images/save"), tr ("Save"), buttons_p);
	buttons_layout_p -> addWidget (btn_p);
	connect (btn_p, &QPushButton :: clicked, this, &ResultsWindow :: SaveResults);


	buttons_p -> setLayout (buttons_layout_p);


	layout_p -> addWidget (buttons_p);

	setLayout (layout_p);
}


uint32 ResultsWindow :: AddAllResultsPagesFromJSON (const json_t *json_p)
{
  return rw_results_p -> AddAllResultsPagesFromJSON (json_p);
}


void ResultsWindow :: SaveResults (bool clicked_flag)
{

}
