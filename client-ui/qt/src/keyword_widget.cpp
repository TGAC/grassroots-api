#include <QVBoxLayout>

#include "keyword_widget.h"


KeywordWidget :: KeywordWidget (QWidget *parent_p, ParameterLevel initial_level)
	: QWidget (parent_p)
{
	QVBoxLayout *layout_p = new QVBoxLayout;
	setLayout (layout_p);

	kw_text_box_p = new QLineEdit;
	connect (kw_text_box_p, &QLineEdit :: returnPressed, this, &KeywordWidget :: TextEntered);
	layout_p -> addWidget (kw_text_box_p);
}


void KeywordWidget :: TextEntered ()
{
	QString search_text = kw_text_box_p -> text ();
	emit RunKeywordSearch (search_text);
}


json_t *KeywordWidget :: GetUserValuesAsJSON (bool full_flag)
{

}
