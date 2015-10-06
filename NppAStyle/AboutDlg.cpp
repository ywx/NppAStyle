/***************************************************************
 * Name:        AboutDlg.cpp
 * Purpose:     Class AboutDlg
 * Author:      YWX (wxFortranIndent@163.com)
 * Created:     2015-7-25
 * Copyright:   (c) YWX <wxFortranIndent@163.com>
 * Licence:     GNU General Public License, version 3
 **************************************************************/

#include "PluginDefinition.h"
#include "AboutDlg.h"
#include "resource.h"


void AboutDlg::doDialog()
{
	if( !isCreated() )
	{
		if( m_isReadMeDlg )
		{
			create( IDD_README_DLG );
		}
		else
		{
			create( IDD_ABOUT_DLG );
		}
	}

	goToCenter();
}


BOOL CALLBACK AboutDlg::run_dlgProc( UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch( Message )
	{
		case WM_INITDIALOG :
			{
				if( m_isReadMeDlg )
				{
					HRSRC hRsrc = ::FindResource( _hInst , MAKEINTRESOURCE(txt_Readme), (LPCTSTR) TEXT("READMEDATA") );
					if( 0 == hRsrc )
					{
						::MessageBox( NULL, TEXT("Error cannot Find \"txt_Readme\" Resource!!!"), TEXT("NppAStyle Message"), MB_ICONSTOP | MB_OK );
						return TRUE;
					}

					HGLOBAL hGlobal = ::LoadResource( _hInst, hRsrc );
					if( 0 == hGlobal )
					{
						::MessageBox( NULL, TEXT("Error cannot Load \"txt_Readme\" Resource!!!"), TEXT("NppAStyle Message"), MB_ICONSTOP | MB_OK );
						return TRUE;
					}

					char * pData = ( char * ) ::LockResource( hGlobal );
					if( pData == 0 )
					{
						::MessageBox( NULL, TEXT("Error cannot Lock \"txt_Readme\" Resource!!!"), TEXT("NppAStyle Message"), MB_ICONSTOP | MB_OK );
						return TRUE;
					}

					HWND hWndCtrl = ::GetDlgItem( _hSelf, IDC_TXT_README );
					::SendMessage( hWndCtrl, SCI_SETCODEPAGE, SC_CP_UTF8, 0 );
					::SendMessage( hWndCtrl, SCI_STYLESETSIZE, STYLE_DEFAULT, 9 );
					::SendMessage( hWndCtrl, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM) "Courier New" );
					::SendMessage( hWndCtrl, SCI_SETMARGINWIDTHN, 1, 0 );
					::SendMessage( hWndCtrl, SCI_USEPOPUP, 0, 0 );
					::SendMessage( hWndCtrl, SCI_SETWRAPMODE, SC_WRAP_WORD, 0 );
					::SendMessage( hWndCtrl, SCI_SETREADONLY, 0, 0 );
					::SendMessage( hWndCtrl, SCI_SETTEXT, 0, ( LPARAM ) pData );
					::SendMessage( hWndCtrl, SCI_SETREADONLY, 1, 0 );
				}
				return TRUE;
			}
			break;
		case WM_COMMAND :
			{
				switch( wParam )
				{
					case IDOK :
					case IDCANCEL :
						{
							display( FALSE );
							return TRUE;
						}
						break;
					default :
						break;
				}
			}
			break;
		default :
			break;
	}
	return FALSE;
}

