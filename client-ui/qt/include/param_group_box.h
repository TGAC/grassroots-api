#ifndef PARAM_GROUP_BOX_H
#define PARAM_GROUP_BOX_H

#include <QFormLayout>
#include <QGroupBox>

#include "base_param_widget.h"


class ParamGroupBox : public QGroupBox
{
public:
	ParamGroupBox (const char * const name_s);

	void AddParameterWidget (BaseParamWidget *widget_p);

	void CheckVisibility (ParameterLevel level);

private:
	QList <BaseParamWidget *> pg_children;
	QFormLayout *pg_layout_p;
};


#endif // PARAM_GROUP_BOX_H
