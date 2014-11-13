#include "browser_widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	BrowserWidget w;

	char * const google_tgac_wheatis_s = "https://accounts.google.com/o/oauth2/auth?scope=email%20profile&redirect_uri=urn:ietf:wg:oauth:2.0:oob&response_type=code&client_id=172491075751-ag0h90hdaml6iv2b7rjanru5fa7mhfm6.apps.googleusercontent.com";
	char * const dropbox_tgac_wheatis_s = "https://www.dropbox.com/1/oauth2/authorize?response_type=code&client_id=65viide1m1ye7pd&state=111111111";

		w.SetUrl (google_tgac_wheatis_s);
w.show();

	return a.exec();
}
