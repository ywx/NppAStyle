/***************************************************************
 * Name:        AboutDlg.h
 * Purpose:     declaration of Class AboutDlg
 * Author:      YWX (wxFortranIndent@163.com)
 * Created:     2015-7-25
 * Copyright:   (c) YWX <wxFortranIndent@163.com>
 * Licence:     GNU General Public License, version 3
 **************************************************************/

#ifndef _ABOUT_DLG_H_
#define _ABOUT_DLG_H_

#include "StaticDialog.h"
#include "PluginInterface.h"


class AboutDlg : public StaticDialog
{

public:
	AboutDlg( bool isReadMeDlg = false ) : StaticDialog(), m_isReadMeDlg( isReadMeDlg ) {};

	void init( HINSTANCE hInst, NppData nppData )
	{
		_nppData = nppData;
		Window::init( hInst, nppData._nppHandle );
	};

	void doDialog();

	virtual void destroy()
	{
	};


protected :
	virtual BOOL CALLBACK run_dlgProc( UINT message, WPARAM wParam, LPARAM lParam );

private:
	NppData			_nppData;
	bool m_isReadMeDlg;
};

#endif // _ABOUT_DLG_H_
