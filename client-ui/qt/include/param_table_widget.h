#ifndef PARAM_TABLE_WIDGET_H
#define PARAM_TABLE_WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "parameter.h"

#include "base_param_widget.h"


class DroppableTableWidget : public QTableWidget
{
public:
	DroppableTableWidget (QWidget *parent_p, char delimiter);

	void SetRow (const int row, const char *data_s);

	void SetDelimiter (char delimiter);
	char GetDelimiter ();

	char *GetValueAsText ();
	bool SetValueFromText (const char *data_s);

protected:
	virtual void dragEnterEvent (QDragEnterEvent *event_p);

	virtual void dropEvent (QDropEvent *event_p);

	void LoadText (const char *filename_s);



private:
	char dtw_delimiter;
};


class ParamTableWidget : public BaseParamWidget
{
	Q_OBJECT

public:
	ParamTableWidget (Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p = 0);
	virtual ~ParamTableWidget ();

	virtual void RemoveConnection ();

	virtual void SetDefaultValue ();

	virtual bool SetValueFromText (const char *value_s);


	virtual bool StoreParameterValue ();

protected:
	DroppableTableWidget *ptw_table_p;
	char ptw_delimiter;

	virtual QWidget *GetQWidget ();
};


#endif // PARAM_TABLE_WIDGET_H

