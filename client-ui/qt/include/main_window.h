#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H


#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMainWindow>

#include "prefs_widget.h"
#include "hash_table.h"


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

public:
	MainWindow (struct QTClientData *data_p);
	~MainWindow ();

	void CreateAndAddServicePage (const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, ParameterSet *params_p);


	json_t *GetUserValuesAsJSON (bool all_flag) const;


protected:
	virtual void dropEvent (QDropEvent *event_p);
	virtual void dragEnterEvent (QDragEnterEvent *event_p);
	virtual void closeEvent (QCloseEvent *event_p);

	void LoadConfigurationFile (QString &filename_r);


private:
	PrefsWidget *mw_prefs_widget_p;
	struct QTClientData *mw_client_data_p;

	void GenerateMenu ();
	void SetParams (json_t *config_p);
};


#endif		/* #ifndef EPR_WINDOW_H */
