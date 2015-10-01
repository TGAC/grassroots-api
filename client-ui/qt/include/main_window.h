#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H


#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMainWindow>

#include "keyword_widget.h"
#include "prefs_widget.h"

struct QTClientData;

class MainWindow : public QMainWindow
{
	Q_OBJECT

signals:
	void Closed ();

public slots:
	void SetBasicInterfaceLevel ();
	void SetIntermediateInterfaceLevel ();
	void SetAdvancedInterfaceLevel ();
	void LoadConfiguration ();
	void SaveConfiguration ();
	void RunServices (bool run_flag);
	void RunKeywordSearch (QString keywords);


public:
	MainWindow (struct QTClientData *data_p);
	virtual ~MainWindow ();

	void CreateAndAddServicePage (const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, const json_t *provider_p, ParameterSet *params_p);


	json_t *GetUserValuesAsJSON (bool all_flag) const;


protected:
	virtual void dropEvent (QDropEvent *event_p);
	virtual void dragEnterEvent (QDragEnterEvent *event_p);
	virtual void closeEvent (QCloseEvent *event_p);

	void LoadConfigurationFile (QString &filename_r);


private:
	PrefsWidget *mw_prefs_widget_p;
	KeywordWidget *mw_keyword_widget_p;
	struct QTClientData *mw_client_data_p;

	void AddActions ();
	void SetParams (json_t *config_p);

	bool AddResults (const json_t *job_results_p);

private slots:
	void Accept ();
	void Reject ();

};


#endif		/* #ifndef EPR_WINDOW_H */
