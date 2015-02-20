#ifndef RESULTS_LIST_HPP
#define RESULTS_LIST_HPP

#include <QListWidget>
#include <QList>
#include <QWebView>

#include "jansson.h"


class ResultsList : public QWidget
{
	Q_OBJECT

signals:

public slots:
	bool SetListFromJSON (const json_t *results_list_json_p);

private slots:
	void OpenItemLink (QListWidgetItem *item_p);

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

	bool AddItemFromJSON (const json_t *json_p);

};

#endif		/* #ifndef RESULTS_LIST_HPP */
