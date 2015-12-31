/***************************************************************
 * Name:        NppAStyleOptionDlg.h
 * Purpose:     declaration of NppAStyleOptionDlg
 * Author:      YWX (wxFortranIndent@163.com)
 * Created:     2015-7-31
 * Copyright:   (c) YWX <wxFortranIndent@163.com>
 * Licence:     GNU General Public License, version 3
 **************************************************************/

#ifndef _NPP_ASTYLE_OPTION_DLG_H_
#define _NPP_ASTYLE_OPTION_DLG_H_

#include "StaticDialog.h"
#include "PluginInterface.h"

struct NppAStyleOption;

class NppAStyleOptionDlg : public StaticDialog
{
	enum enumIndexOptionSet { indexBracketModifyOptions, indexIndentationOptions, indexPaddingOptions, indexFormattingOptions, indexObjectiveCOptions, indexOptionSet_Len };

public:
	NppAStyleOptionDlg() : StaticDialog(), astyleOption( NULL ), m_astyleOption( NULL ), m_indexOptionSet( 0 ) {};
	~NppAStyleOptionDlg();

	void init( HINSTANCE hInst, NppData nppData, NppAStyleOption *pAstyleOption )
	{
		_nppData = nppData;
		astyleOption = pAstyleOption;
		Window::init( hInst, nppData._nppHandle );
	};

	void doDialog();

	inline HWND GetDlgItem( const UINT id ) { return ::GetDlgItem( _hSelf, id ); }
	inline LONG SendDlgItemMessage( const UINT id, UINT Msg, WPARAM wParam, LPARAM lParam )
	{
		return ::SendDlgItemMessage( _hSelf, id, Msg, wParam, lParam );
	}

protected :
	virtual BOOL CALLBACK run_dlgProc( UINT message, WPARAM wParam, LPARAM lParam );
	void optionImportExport( bool isImport );

	BOOL CALLBACK DlgOptionProc( UINT Message, WPARAM wParam, LPARAM lParam );

	void initDlgOptionControl();
	void showDlgOptionControl();
	void showDlgControlRange( const UINT idStart, const UINT idEnd, const bool isShow, const bool isEnable = true );
	void initDlgComboList();
	void initPreviewCtrl();
	void initDlgControl();
	void updateDlgTabsetting();
	void updateDlgPreviewText();

private:
	/* Handles */
	NppData _nppData;

	NppAStyleOption *astyleOption;
	NppAStyleOption *m_astyleOption;
	static const char *m_textPreviewCode;

	int m_indexOptionSet;
};

#endif // _NPP_ASTYLE_OPTION_DLG_H_
