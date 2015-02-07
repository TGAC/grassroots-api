#include <QVBoxLayout>

#include "results_list.h"



ResultsList :: ResultsList (QWidget *parent_p)
	:	QWidget (parent_p)
{
	QLayout *layout_p = new QVBoxLayout;

	rl_list_p = new QListWidget;

	layout_p -> addWidget (rl_list_p);

	setLayout (layout_p);
}


ResultsList :: ~ResultsList ()
{

}


void ResultsList :: SetListFromJSON (const json_t *results_list_json_p)
{

}
