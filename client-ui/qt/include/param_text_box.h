#ifndef PARAM_TEXT_BOX_H
#define PARAM_TEXT_BOX_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "parameter.h"

#include "base_param_widget.h"


class DroppableTextBox : public QPlainTextEdit
{
public:
	DroppableTextBox (QWidget *parent_p);

protected:
	virtual void dragEnterEvent (QDragEnterEvent *event_p);

	virtual void dropEvent (QDropEvent *event_p);

	void LoadText (const char *filename_s);
};


class ParamTextBox : public BaseParamWidget
{
	Q_OBJECT


public:
	ParamTextBox (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p = 0);
	virtual ~ParamTextBox ();

	virtual void RemoveConnection ();

	virtual void SetDefaultValue ();

	virtual bool SetValueFromText (const char *value_s);

	virtual bool StoreParameterValue ();

protected:
	DroppableTextBox *ptb_text_box_p;

	virtual QWidget *GetQWidget ();
};




#endif // PARAM_TEXT_BOX_H
