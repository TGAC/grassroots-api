#ifndef PARAMETER_LINE_EDIT_H
#define PARAMETER_LINE_EDIT_H


#include <QWidget>
#include <QLineEdit>

#include "parameter.h"

#include "base_param_widget.h"



class ParamLineEdit : public BaseParamWidget
{
  Q_OBJECT


private slots:
  bool UpdateConfig ();
  bool UpdateConfigValue (const char * const value_s);

public:
  ParamLineEdit (Parameter * const param_p, const PrefsWidget * const options_widget_p, QLineEdit :: EchoMode echo = QLineEdit :: Normal, QWidget *parent_p = 0);
  virtual ~ParamLineEdit ();

  virtual void RemoveConnection ();

  virtual void SetDefaultValue ();

  virtual bool SetValueFromText (const char *value_s);

protected:
  QLineEdit *ple_text_box_p;

  virtual QWidget *GetQWidget ();


};



#endif		/* #ifndef PARAMETER_LINE_EDIT_H */
