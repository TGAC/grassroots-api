#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMimeData>
#include <QDebug>


#include "results_window.h"


ResultsWindow :: ResultsWindow (QMainWindow *parent_p)
	: rw_data_p (0)
{
	QVBoxLayout *layout_p = new QVBoxLayout;

	rw_results_p = new ResultsWidget;
	layout_p -> addWidget (rw_results_p);

	QGroupBox *buttons_p = new QGroupBox;
	QHBoxLayout *buttons_layout_p = new QHBoxLayout;

	QPushButton *btn_p = new QPushButton (QIcon ("images/ok"), tr ("Ok"), buttons_p);
	buttons_layout_p -> addWidget (btn_p);
	connect (btn_p, &QPushButton :: clicked, this, &ResultsWindow :: close);

	btn_p = new QPushButton (QIcon ("images/save"), tr ("Save"), buttons_p);
	buttons_layout_p -> addWidget (btn_p);
	connect (btn_p, &QPushButton :: clicked, this, &ResultsWindow :: SaveResults);


	buttons_p -> setLayout (buttons_layout_p);

	layout_p -> addWidget (buttons_p);

	setWindowTitle (tr ("Results"));
	setWindowIcon (QIcon ("images/viewas_list"));
	setLayout (layout_p);

	setAcceptDrops (true);
}


void ResultsWindow :: ClearData ()
{
/*
  if (rw_data_p)
    {
      json_array_clear (rw_data_p);
      json_decref (rw_data_p);
    }
*/
}


ResultsWindow :: ~ResultsWindow ()
{
  ClearData ();
}


uint32 ResultsWindow :: AddAllResultsPagesFromJSON (const json_t *json_p)
{  
  ClearData ();
  rw_data_p = json_p;

  return rw_results_p -> AddAllResultsPagesFromJSON (json_p);
}


void ResultsWindow :: SaveResults (bool clicked_flag)
{
	QString filename = QFileDialog :: getSaveFileName (this, tr ("Save Results"), "wheatis_results.json", tr ("JSON (*.json)"));

	if (! (filename.isNull () || filename.isEmpty ()))
		{
			QByteArray ba = filename.toLocal8Bit ();
			const char * const filename_s = ba.constData ();

			int res = json_dump_file (rw_data_p, filename_s, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

			if (res != 0)
				{

				}

		}		/* if (! (filename.isNull () || filename.isEmpty ())) */

}


void ResultsWindow :: LoadResults (const char * const filename_s)
{
	json_error_t error;

	json_t *results_p =	json_load_file (filename_s, 0, &error);

	if (results_p)
		{
			rw_results_p -> AddAllResultsPagesFromJSON (results_p);
		}
}


void ResultsWindow :: LoadResults ()
{
	QString filename = QFileDialog :: getSaveFileName (this, tr ("Load Results"), "wheatis_results.json", tr ("JSON (*.json)"));

	if (! (filename.isNull () || filename.isEmpty ()))
		{
			QByteArray ba = filename.toLocal8Bit ();
			const char * const filename_s = ba.constData ();

			LoadResults (filename_s);
		}		/* if (! (filename.isNull () || filename.isEmpty ())) */

}


void ResultsWindow :: dropEvent (QDropEvent *event_p)
{
	QList <QUrl> urls = event_p -> mimeData () -> urls ();

	if (! (urls.isEmpty ()))
		{
			QString filename = urls.first ().toLocalFile ();

			if (! ((filename.isEmpty ()) || (filename.isNull ())))
				{
					QByteArray ba = filename.toLocal8Bit ();
					const char * const filename_s = ba.constData ();

					qDebug () << "dropped " << filename;

					LoadResults (filename_s);
				}		/* if (! (filename.isEmpty ())) */

		}		/* if (! (urls.isEmpty ())) */

}


void ResultsWindow :: dragEnterEvent (QDragEnterEvent *event_p)
{
	event_p -> acceptProposedAction ();
}

