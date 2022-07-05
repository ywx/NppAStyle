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

struct NppAStyleOptionSet;

class NppAStyleOptionDlg : public StaticDialog
{
	enum enumIndexOptionSet { indexBraceModifyOptions, indexIndentationOptions, indexPaddingOptions, indexFormattingOptions, indexObjectiveCOptions, indexOptionSet_Len };

public:
	NppAStyleOptionDlg() : StaticDialog(), astyleOptionSet( NULL ), m_astyleOption( NULL ), m_languageMode( 0 ), m_indexOptionSet( 0 ) {};
	~NppAStyleOptionDlg();

	void init( HINSTANCE hInst, NppData nppData, NppAStyleOptionSet *pAstyleOptionSet )
	{
		_nppData = nppData;
		astyleOptionSet = pAstyleOptionSet;
		m_textPreviewCode = 0;
		Window::init( hInst, nppData._nppHandle );
	};

	void doDialog();

	inline HWND GetDlgItem( const UINT id ) { return ::GetDlgItem( _hSelf, id ); }
	inline INT_PTR SendDlgItemMessage( const UINT id, UINT Msg, WPARAM wParam, LPARAM lParam )
	{
		return ::SendDlgItemMessage( _hSelf, id, Msg, wParam, lParam );
	}

protected :
	virtual INT_PTR CALLBACK run_dlgProc( UINT message, WPARAM wParam, LPARAM lParam );
	void optionImportExport( bool isImport );

	INT_PTR CALLBACK DlgOptionProc( UINT Message, WPARAM wParam, LPARAM lParam );

	void initDlgOptionControl();
	void showDlgOptionControl();
	void showDlgControlRange( const UINT idStart, const UINT idEnd, const bool isShow, const bool isEnable = true );
	void initDlgComboList();
	void initPreviewCtrl();
	void initDlgControl();
	void updateDlgTabsetting();
	void updateDlgLangSetting();
	void updateDlgPreviewText();

private:
	/* Handles */
	NppData _nppData;

	NppAStyleOptionSet *astyleOptionSet;
	NppAStyleOption *m_astyleOption;
	const char *m_textPreviewCode;

	int m_languageMode;

	int m_indexOptionSet;
};

#endif // _NPP_ASTYLE_OPTION_DLG_H_
