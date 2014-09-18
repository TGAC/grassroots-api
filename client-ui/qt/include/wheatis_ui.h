#ifndef WHEATISUI_H
#define WHEATISUI_H

#include <QWidget>

#include "jansson.h"

class WheatISUI : public QWidget
{
	Q_OBJECT
public:
	explicit WheatISUI(QWidget *parent = 0);

	bool BuildUI (const json_t *json_p);

signals:

public slots:

};

#endif // WHEATISUI_H
