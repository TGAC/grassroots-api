#ifndef RESULTS_LIST_HPP
#define RESULTS_LIST_HPP

#include <QListWidget>

#include "jansson.h"


class ResultsList : public QWidget
{
	Q_OBJECT

signals:

public slots:
	void SetListFromJSON (const json_t *results_list_json_p);

public:

	/*********************/
	/***** FUNCTIONS *****/
	/*********************/

	ResultsList (QWidget *parent_p);

	~ResultsList ();


private:

	/*********************/
	/***** VARIABLES *****/
	/*********************/
	QListWidget *rl_list_p;
};

#endif		/* #ifndef RESULTS_LIST_HPP */
