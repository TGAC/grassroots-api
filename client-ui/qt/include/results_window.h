#ifndef RESULTS_WINDOW_H
#define RESULTS_WINDOW_H

#include <QMainWindow>
#include <QWindow>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "results_widget.h"

#include "typedefs.h"
#include "jansson.h"

class ResultsWindow : public QWidget
{
public:
  ResultsWindow (QMainWindow *parent_p);
  ~ResultsWindow ();
	uint32 AddAllResultsPagesFromJSON (json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s);
	bool AddResultsPageFromJSON (json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s);
	void ClearData ();

protected:
	virtual void dropEvent (QDropEvent *event_p);
	virtual void dragEnterEvent (QDragEnterEvent *event_p);

private:

	bool AddData (json_t *data_p);

	void LoadResults (const char * const filename_s);
	void LoadResults ();



private slots:
  void SaveResults (bool clicked_flag);

private:
  ResultsWidget *rw_results_p;
	json_t *rw_data_p;
};


#endif // RESULTS_WINDOW_H

