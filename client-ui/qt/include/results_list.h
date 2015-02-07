#ifndef RESULTS_LIST_HPP
#define RESULTS_LIST_HPP

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

	~PrefsWidget ();


private:

	/*********************/
	/***** VARIABLES *****/
	/*********************/

	ParameterLevel pw_level;
	QTabWidget *pw_tabs_p;
	QList <ServicePrefsWidget *> pw_service_widgets;

};

#endif		/* #ifndef RESULTS_LIST_HPP */
