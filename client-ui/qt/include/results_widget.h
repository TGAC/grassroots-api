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

  uint32 AddAllResultsPagesFromJSON (const json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s);
  bool AddResultsPageFromJSON (const json_t *json_p, const char * const service_name_s, const char * const service_description_s, const char * const service_uri_s);

private:
  QWidget *CreatePageFromJSON (const json_t *json_p, const char * const description_s, const char * const uri_s);

};

#endif // RESULTS_WIDGET_H
