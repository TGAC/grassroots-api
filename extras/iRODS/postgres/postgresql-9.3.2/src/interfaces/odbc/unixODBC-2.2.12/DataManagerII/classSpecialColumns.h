/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 18.FEB.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey   - pharvey@codebydesign.com
 **************************************************/
#ifndef classSpecialColumns_included
#define classSpecialColumns_included

#include "classCanvas.h"
#include "classColumn.h"
#include <qstring.h>
#include <qlist.h>
#include <qlistview.h>
#include <sqlext.h>

class classSpecialColumns: public classNode
{
public:
   classSpecialColumns( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc, const char *pszTable = 0, const char *pszLibrary = 0 );
  ~classSpecialColumns() {}

   void setOpen( bool );

private:
  QList<classColumn>  listColumns;
  SQLHDBC             hDbc;
  QString             qsLibrary;
  QString             qsTable;

  void LoadColumns();

};
#endif

