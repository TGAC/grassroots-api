#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H


#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMainWindow>

#include "prefs_widget.h"
#include "hash_table.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT


public slots:
	void SetBasicInterfaceLevel ();
	void SetIntermediateInterfaceLevel ();
	void SetAdvancedInterfaceLevel ();
	void LoadConfiguration ();
	void SaveConfiguration ();

public:
	MainWindow ();
	~MainWindow ();

	void CreateAndAddServicePage (const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, ParameterSet *params_p);


	json_t *GetUserValuesAsJSON (bool all_flag) const;


protected:
	virtual void dropEvent (QDropEvent *event_p);
	virtual void dragEnterEvent (QDragEnterEvent *event_p);

	void LoadConfigurationFile (QString &filename_r);


private:
	PrefsWidget *pw_prefs_widget_p;

	void GenerateMenu ();
};


#endif		/* #ifndef EPR_WINDOW_H */
