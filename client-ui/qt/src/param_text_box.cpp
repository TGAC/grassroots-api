#include "param_text_box.h"
#include "prefs_widget.h"

#include "string_utils.h"


ParamTextBox :: ParamTextBox (const Parameter * const param_p, const PrefsWidget * const options_widget_p, QWidget *parent_p)
:		BaseParamWidget (param_p, options_widget_p)
{
	ptb_text_box_p = new QLineEdit (parent_p);

	connect (ptb_text_box_p, &QLineEdit :: textChanged, this, &ParamTextBox :: UpdateConfig);
}


ParamTextBox ::	~ParamTextBox ()
{
	delete ptb_text_box_p;
	ptb_text_box_p = NULL;
}


void ParamTextBox :: RemoveConnection ()
{
	disconnect (ptb_text_box_p, &QLineEdit :: textChanged, this, &ParamTextBox :: UpdateConfig);
}



QWidget *ParamTextBox :: GetQWidget ()
{
	return ptb_text_box_p;
}

