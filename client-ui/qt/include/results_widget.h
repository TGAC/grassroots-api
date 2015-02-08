#ifndef RESULTS_WIDGET_H
#define RESULTS_WIDGET_H

#include <QTabWidget>

#include "jansson.h"
#include "typedefs.h"

class ResultsWidget : public QTabWidget
{
public:
  ResultsWidget (QWidget *parent_p = 0);
  ~ResultsWidget ();

  uint32 AddAllResultsPagesFromJSON (const json_t *json_p);
  bool AddResultsPageFromJSON (const json_t *json_p);
};

#endif // RESULTS_WIDGET_H
