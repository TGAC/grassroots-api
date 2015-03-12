#ifndef RESULTS_WINDOW_H
#define RESULTS_WINDOW_H

#include <QMainWindow>
#include <QWindow>

#include "results_widget.h"

#include "typedefs.h"
#include "jansson.h"

class ResultsWindow : public QWidget
{
public:
  ResultsWindow (QMainWindow *parent_p);
  uint32 AddAllResultsPagesFromJSON (const json_t *json_p);


private slots:
  void SaveResults (bool clicked_flag);


private:
  ResultsWidget *rw_results_p;
};


#endif // RESULTS_WINDOW_H

