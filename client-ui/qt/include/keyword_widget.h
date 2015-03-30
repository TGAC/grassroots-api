#ifndef KEYWORD_WIDGET_H
#define KEYWORD_WIDGET_H

#include <QWidget>
#include <QLineEdit>

#include "parameter.h"

#include "runnable_widget.h"


class KeywordWidget : public QWidget, public RunnableWidget
{
	Q_OBJECT

public:
	KeywordWidget (QWidget *parent_p, ParameterLevel initial_level);

	virtual json_t *GetUserValuesAsJSON (bool full_flag);

signals:
	void RunKeywordSearch (QString s);

protected slots:
	void TextEntered ();

private:
	QLineEdit *kw_text_box_p;


};

#endif // KEYWORD_WIDGET_H
