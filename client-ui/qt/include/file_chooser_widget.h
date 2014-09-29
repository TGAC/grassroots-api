#ifndef FILE_CHOOSER_WIDGET_H
#define FILE_CHOOSER_WIDGET_H

#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>

#include "base_param_widget.h"

class FileChooserWidget : public BaseParamWidget
{
	Q_OBJECT

private slots:
	void ChooseFile ();
	bool UpdateConfig (const QString &value_r);


public:
	FileChooserWidget (Parameter * const param_p, const PrefsWidget * const prefs_widget_p, QFileDialog :: FileMode mode);

	~FileChooserWidget ();


	virtual QWidget *GetQWidget ();

	virtual void SetDefaultValue ();

private:
	QWidget *fcw_widget_p;
	QComboBox *fcw_chooser_p;
	QString fcw_title;
	QFileDialog :: FileMode fcw_file_mode;
	QFileDialog :: AcceptMode fcw_accept_mode;
};

#endif		/* #ifndef FILE_CHOOSER_WIDGET_H */
