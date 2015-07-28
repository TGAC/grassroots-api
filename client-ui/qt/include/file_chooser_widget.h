#ifndef FILE_CHOOSER_WIDGET_H
#define FILE_CHOOSER_WIDGET_H

#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>

#include "base_param_widget.h"

class FileChooserWidget : public BaseParamWidget
{
	Q_OBJECT

private slots:
	void ChooseFile ();


public:
	FileChooserWidget (Parameter * const param_p, const PrefsWidget * const prefs_widget_p, QFileDialog :: FileMode mode);

	~FileChooserWidget ();

	virtual void SetDefaultValue ();

	virtual bool SetValueFromText (const char *value_s);

	virtual bool StoreParameterValue ();

protected:
	virtual QWidget *GetQWidget ();

private:
	QWidget *fcw_widget_p;
	QComboBox *fcw_chooser_p;
	QLabel *fcw_protocol_label_p;
	QString fcw_title;
	QFileDialog :: FileMode fcw_file_mode;
	QFileDialog :: AcceptMode fcw_accept_mode;
};

#endif		/* #ifndef FILE_CHOOSER_WIDGET_H */
