/***************************************************************
 * Name:        NppAStyleOptionDlg.cpp
 * Purpose:     Class NppAStyleOptionDlg
 * Author:      YWX (wxFortranIndent@163.com)
 * Created:     2015-7-31
 * Copyright:   (c) YWX <wxFortranIndent@163.com>
 * Licence:     GNU General Public License, version 3
 **************************************************************/

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include <stdio.h>
#include <CommCtrl.h>
#include <string>
#include "NppAStyleOption.h"
#include "NppAStyleOptionDlg.h"
#include "resource.h"

enum enumIndexTabSize { indexTabSize_2, indexTabSize_3, indexTabSize_4, indexTabSize_5, indexTabSize_6, indexTabSize_8, indexTabSize_10, indexTabSize_Len };
static TCHAR listTabSize[indexTabSize_Len][4] = { TEXT( "2" ), TEXT( "3" ), TEXT( "4" ), TEXT( "5" ), TEXT( "6" ), TEXT( "8" ), TEXT( "10" ) };
static int listTabSizeValue[indexTabSize_Len] = { 2, 3, 4, 5, 6, 8, 10 };

static int Value2Index( const int value )
{
	for( int i = 0; i < indexTabSize_Len; ++i )
	{
		if( value == listTabSizeValue[i] )
			return i;
	}
	return indexTabSize_4;
}


NppAStyleOptionDlg::~NppAStyleOptionDlg()
{
	if( isCreated() )
	{
		if( m_astyleOption != NULL )
			delete m_astyleOption;
	}
}

void NppAStyleOptionDlg::initDlgComboList()
{
	HWND hWndComboBox = GetDlgItem( IDC_CBB_TABSIZE );
	TCHAR strLabel[32];
	memset( &strLabel, 0, sizeof( strLabel ) );
	for( int i = 0; i < indexTabSize_Len ; ++i )
	{
		_tcscpy( strLabel, listTabSize[i] );
		::SendMessage( hWndComboBox, ( UINT ) CB_ADDSTRING, 0, ( LPARAM ) strLabel );
	}

	hWndComboBox = GetDlgItem( IDC_CBB_ASTYLE_LANG_MODE );
	for( int i = 0; i < astyleOptionSet->languageCount; ++i )
	{
		::SendMessage( hWndComboBox, ( UINT ) CB_ADDSTRING, 0, ( LPARAM ) astyleOptionSet->languageNames[i] );
	}
	SendDlgItemMessage( IDC_CBB_ASTYLE_LANG_MODE, CB_SETCURSEL, ( WPARAM ) m_languageMode, 0 );

	TCHAR *listBraceStyle[] = { TEXT( "None" ), TEXT( "Allman (ANSI)" ), TEXT( "Java" ), TEXT( "Kernighan & Ritchie (K&R)" ), TEXT( "Stroustrup" ), TEXT( "Whitesmith" ), TEXT( "VTK" ), TEXT( "Ratliff" ), TEXT( "GNU" ), TEXT( "Linux" ), TEXT( "Horstmann" ), TEXT( "One True Brace (1TBS)" ), TEXT( "Google" ), TEXT( "Mozilla" ), TEXT( "Pico" ), TEXT( "Lisp" ), 0 };
#define FormattingStyleCount sizeof(listBraceStyle)/sizeof(TCHAR *)-1
	hWndComboBox = GetDlgItem( IDC_CBB_BRACKET_STYLE );
	for( int i = 0; i < FormattingStyleCount ; ++i )
	{
		_tcscpy( strLabel, listBraceStyle[i] );
		::SendMessage( hWndComboBox, ( UINT ) CB_ADDSTRING, 0, ( LPARAM ) strLabel );
	}

	TCHAR *listOptionSet[] = { TEXT( "Brace Modify Options" ), TEXT( "Indentation Options" ), TEXT( "Padding Options" ), TEXT( "Formatting Options" ), TEXT( "Objective-C Options" ), 0 };
	hWndComboBox = GetDlgItem( IDC_CBB_ASTYLE_OPTION_SET );
	for( int i = 0; i < 5 ; ++i )
	{
		_tcscpy( strLabel, listOptionSet[i] );
		::SendMessage( hWndComboBox, ( UINT ) CB_ADDSTRING, 0, ( LPARAM ) strLabel );
	}

	TCHAR *listMinConditional[] = { TEXT( "Zero" ), TEXT( "One" ), TEXT( "Two" ), TEXT( "One Half" ), 0 };
	TCHAR *listPtrAlign[] = { TEXT( "None" ), TEXT( "Type" ), TEXT( "Middle" ), TEXT( "Name" ), 0 };
	TCHAR *listRefAlign[] = { TEXT( "None" ), TEXT( "Type" ), TEXT( "Middle" ), TEXT( "Name" ), TEXT( "Same as Pointer" ), 0 };
	TCHAR *listObjCColonPad[] = { TEXT( "No Change" ), TEXT( "None" ), TEXT( "All" ), TEXT( "After" ), TEXT( "Before" ), 0 };

	hWndComboBox = GetDlgItem( IDC_CBB_MinConditionalIndent );
	for( int i = 0; i < 4 ; ++i )
	{
		_tcscpy( strLabel, listMinConditional[i] );
		::SendMessage( hWndComboBox, ( UINT ) CB_ADDSTRING, 0, ( LPARAM ) strLabel );
	}

	hWndComboBox = GetDlgItem( IDC_CBB_PtrAlign );
	for( int i = 0; i < 4 ; ++i )
	{
		_tcscpy( strLabel, listPtrAlign[i] );
		::SendMessage( hWndComboBox, ( UINT ) CB_ADDSTRING, 0, ( LPARAM ) strLabel );
	}

	hWndComboBox = GetDlgItem( IDC_CBB_RefAlign );
	for( int i = 0; i < 5 ; ++i )
	{
		_tcscpy( strLabel, listRefAlign[i] );
		::SendMessage( hWndComboBox, ( UINT ) CB_ADDSTRING, 0, ( LPARAM ) strLabel );
	}

	hWndComboBox = GetDlgItem( IDC_CBB_ObjC_ColonPad );
	for( int i = 0; i < 5 ; ++i )
	{
		_tcscpy( strLabel, listObjCColonPad[i] );
		::SendMessage( hWndComboBox, ( UINT ) CB_ADDSTRING, 0, ( LPARAM ) strLabel );
	}

	hWndComboBox = GetDlgItem( IDC_CBB_ContinuationIndent );
	for( int i = 0; i < 5 ; ++i )
	{
		// wsprintf( strLabel, TEXT("%d"), i );
		_itot( i, strLabel, 10 );
		::SendMessage( hWndComboBox, ( UINT ) CB_ADDSTRING, 0, ( LPARAM ) strLabel );
	}

	hWndComboBox = GetDlgItem( IDC_CBB_MaxContinuationIndent );
	for( int i = 40; i < 121 ; i += 10 )
	{
		// wsprintf( strLabel, TEXT("%d"), i );
		_itot( i, strLabel, 10 );
		::SendMessage( hWndComboBox, ( UINT ) CB_ADDSTRING, 0, ( LPARAM ) strLabel );
	}

	hWndComboBox = GetDlgItem( IDC_CBB_MaxCodeLength );
	for( int i = 40; i < 201 ; i += 10 )
	{
		if( i == 40 )
		{
			_tcscpy( strLabel, TEXT( "Do not change" ) );
		}
		else
		{
			// wsprintf( strLabel, TEXT("%d"), i );
			_itot( i, strLabel, 10 );
		}
		::SendMessage( hWndComboBox, ( UINT ) CB_ADDSTRING, 0, ( LPARAM ) strLabel );
	}
}

HWND getNppCurrentScintilla();

void NppAStyleOptionDlg::updateDlgTabsetting()
{
	if( m_astyleOption->isSameAsNppCurView )
	{
		HWND hWndNppCurrentScintilla = getNppCurrentScintilla();
		m_astyleOption->iTabSize = ::SendMessage( hWndNppCurrentScintilla, SCI_GETTABWIDTH, 0, 0 );
		m_astyleOption->isUseSpace = 0 == ::SendMessage( hWndNppCurrentScintilla, SCI_GETUSETABS, 0, 0 );
	}
	SendDlgItemMessage( IDC_TXT_PREVIEW, SCI_SETTABWIDTH, (WPARAM) m_astyleOption->iTabSize, 0 );
	SendDlgItemMessage( IDC_CHK_UseSpace, BM_SETCHECK, m_astyleOption->isUseSpace, 0 );
	SendDlgItemMessage( IDC_CBB_TABSIZE, CB_SETCURSEL, ( WPARAM ) Value2Index( m_astyleOption->iTabSize ), 0 );

	bool isEnable = ! m_astyleOption->isSameAsNppCurView;
	::EnableWindow( GetDlgItem( IDC_CHK_UseSpace ), isEnable );
	::EnableWindow( GetDlgItem( IDC_CBB_TABSIZE ), isEnable );
}

int getNppCurrentLangId();

void NppAStyleOptionDlg::updateDlgLangSetting(bool isUpdateGUI)
{
	int langType = getNppCurrentLangId();
	if( langType == L_C )
	{
		m_languageMode = 0;
	}
	else if( langType == L_CPP )
	{
		m_languageMode = 1;
	}
	else if( langType == L_JAVA )
	{
		m_languageMode = 2;
	}
	else if( langType == L_CS )
	{
		m_languageMode = 3;
	}
	else if( langType == L_OBJC )
	{
		m_languageMode = 4;
	}

	if( isUpdateGUI )
	{
		SendDlgItemMessage( IDC_CBB_ASTYLE_LANG_MODE, CB_SETCURSEL, ( WPARAM ) m_languageMode, 0 );
	}
}

#define SCLEX_CPP 3
#define SCE_C_DEFAULT 0
#define SCE_C_COMMENT 1
#define SCE_C_COMMENTLINE 2
#define SCE_C_COMMENTDOC 3
#define SCE_C_NUMBER 4
#define SCE_C_WORD 5
#define SCE_C_STRING 6
#define SCE_C_CHARACTER 7
#define SCE_C_UUID 8
#define SCE_C_PREPROCESSOR 9
#define SCE_C_OPERATOR 10
#define SCE_C_IDENTIFIER 11
#define SCE_C_STRINGEOL 12
#define SCE_C_VERBATIM 13
#define SCE_C_REGEX 14
#define SCE_C_COMMENTLINEDOC 15
#define SCE_C_WORD2 16
#define SCE_C_COMMENTDOCKEYWORD 17
#define SCE_C_COMMENTDOCKEYWORDERROR 18
#define SCE_C_GLOBALCLASS 19
#define SCE_C_STRINGRAW 20
#define SCE_C_TRIPLEVERBATIM 21
#define SCE_C_HASHQUOTEDSTRING 22
#define SCE_C_PREPROCESSORCOMMENT 23
#define SCE_C_PREPROCESSORCOMMENTDOC 24
#define SCE_C_USERLITERAL 25
#define SCE_C_TASKMARKER 26
#define SCE_C_ESCAPESEQUENCE 27

#define SCFIND_REGEXP_DOTMATCHESNL    0x10000000

int getNppVersion();

	static const char cppKeyWords[] =
	    "and and_eq asm auto bitand bitor bool break "
	    "case catch char char16_t char32_t class compl const const_cast constexpr continue "
	    "default decltype delete do double dynamic_cast else enum explicit export extern "
	    "false final float for friend goto if inline int long "
	    "mutable namespace new noexcept not not_eq nullptr "
	    "operator or or_eq override private protected public register reinterpret_cast "
	    "return short signed sizeof static static_assert static_cast struct switch "
	    "template this thread_local throw true try typedef typeid typename union unsigned using "
	    "virtual void volatile wchar_t while xor xor_eq ";

	static const char cppTypeWords[] =
	    "std string wstring NULL va_list FILE EOF size_t fpos_t jmp_buf clock_t time_t tm _utimbuf "
	    "_complex _dev_t div_t ldiv_t _exception "
	    "lconv _off_t ptrdiff_t sig_atomic_t _stat "
	    "__cdecl __stdcall __fastcall __declspec dllexport dllexport noked noreturn nothrow onvtable "
	    "__int8 __int16 __int32 __int64 ";

static void stylingFindText( HWND hWndPreviewCtrl, const std::string &strFind, const int searchFlags, const int styleId )
{
	long posEnd = ::SendMessage( hWndPreviewCtrl, SCI_GETLENGTH, 0, 0 );

	if( 0 == posEnd )
	{
		return;
	}

	if( strFind.empty() )
	{
		return;
	}

	char *pStrFind = NULL;
	pStrFind = ( char * ) strFind.c_str();
	size_t lenStrFind = 0;
	lenStrFind = strFind.size();

	long lPos = 0;
	long lPosEnd = 0;

	::SendMessage( hWndPreviewCtrl, SCI_SETSEARCHFLAGS, searchFlags, 0 );
	::SendMessage( hWndPreviewCtrl, SCI_SETTARGETRANGE, 0, posEnd );
	lPos = ::SendMessage( hWndPreviewCtrl, SCI_SEARCHINTARGET, lenStrFind, (LPARAM) pStrFind );
	if( lPos == -2 )
	{
		return; // Invalid regular expression
	}
	while( lPos >= 0 )
	{
		lPosEnd = ::SendMessage( hWndPreviewCtrl, SCI_GETTARGETEND, 0, 0 );
		if(lPosEnd > posEnd) break;
		::SendMessage( hWndPreviewCtrl, SCI_STARTSTYLING, lPos, 0 );
		::SendMessage( hWndPreviewCtrl, SCI_SETSTYLING, lPosEnd - lPos, styleId );

		posEnd = ::SendMessage( hWndPreviewCtrl, SCI_GETLENGTH, 0, 0 );
		if(lPosEnd >= posEnd) break;
		::SendMessage( hWndPreviewCtrl, SCI_SETTARGETRANGE, lPosEnd, posEnd );
		lPos = ::SendMessage( hWndPreviewCtrl, SCI_SEARCHINTARGET, lenStrFind, (LPARAM) pStrFind );
	}
}

static void previewRunProcCallback( const char *in, const char *out, HWND hwin )
{
	::SendMessage( hwin, SCI_SETTEXT, 0, ( LPARAM ) out );
}

void NppAStyleOptionDlg::updateDlgPreviewText()
{
	HWND hWndPreviewCtrl = GetDlgItem( IDC_TXT_PREVIEW );
	::SendMessage( hWndPreviewCtrl, SCI_SETREADONLY, 0, 0 );
	if( m_astyleOption->formattingStyle == 0 )
	{
		::SendMessage( hWndPreviewCtrl, SCI_SETWRAPMODE, SC_WRAP_WORD, 0 );
		::SendMessage( hWndPreviewCtrl, SCI_SETMARGINWIDTHN, 0, 0 );
		::SendMessage( hWndPreviewCtrl, SCI_SETTEXT, 0, (LPARAM) "None style nothing to do." );
	}
	else
	{
		unsigned int pos = ::SendMessage( hWndPreviewCtrl, SCI_GETCURRENTPOS, 0, 0 );
		::SendMessage( hWndPreviewCtrl, SCI_SETWRAPMODE, SC_WRAP_NONE, 0 );
		LPARAM lw = ::SendMessage( hWndPreviewCtrl, SCI_TEXTWIDTH, STYLE_LINENUMBER, (LPARAM) "_999" );
		::SendMessage( hWndPreviewCtrl, SCI_SETMARGINWIDTHN, 0, lw );
		AStyleCode( m_textPreviewCode, * m_astyleOption, previewRunProcCallback, hWndPreviewCtrl );
		::SendMessage( hWndPreviewCtrl, SCI_GOTOPOS, pos, 0 );
	}

	if (getNppVersion() > ((0x08 << 16) | (33)) )
	{
	::SendMessage( hWndPreviewCtrl, SCI_STARTSTYLING, 0, 0 );
	::SendMessage( hWndPreviewCtrl, SCI_SETSTYLING, ::SendMessage( hWndPreviewCtrl, SCI_GETLENGTH, 0, 0 ), SCE_C_DEFAULT );

	stylingFindText( hWndPreviewCtrl, "\\b\\d+\\b", SCFIND_REGEXP, SCE_C_NUMBER );
	std::string strKeyWord("\\b(?:");
	strKeyWord.append(cppKeyWords);
	char * pStr =(char *) strKeyWord.c_str();
	strKeyWord.pop_back();
	while ( ( pStr = strchr( pStr, ' ' ) ) != NULL )
	{
		* pStr = '|';
		++pStr;
	}
	strKeyWord.append(")\\b");
	stylingFindText( hWndPreviewCtrl, strKeyWord, SCFIND_REGEXP | SCFIND_MATCHCASE, SCE_C_WORD );
	stylingFindText( hWndPreviewCtrl, "'.'", SCFIND_REGEXP, SCE_C_STRING );
	stylingFindText( hWndPreviewCtrl, "\".+?\"", SCFIND_REGEXP, SCE_C_STRING );
	stylingFindText( hWndPreviewCtrl, "#\\s*include\\s*[\"<][.A-Z0-9a-z_-]+[\">]", SCFIND_REGEXP | SCFIND_MATCHCASE, SCE_C_PREPROCESSOR );
	stylingFindText( hWndPreviewCtrl, "/\\*.+?\\*/", SCFIND_REGEXP | SCFIND_REGEXP_DOTMATCHESNL, SCE_C_COMMENT );
	stylingFindText( hWndPreviewCtrl, "//.+$", SCFIND_REGEXP, SCE_C_COMMENTLINE );
	}

	::SendMessage( hWndPreviewCtrl, SCI_SETREADONLY, 1, 0 );
}

void NppAStyleOptionDlg::initPreviewCtrl()
{
	HWND hWndPreviewCtrl = GetDlgItem( IDC_TXT_PREVIEW );
	::SendMessage( hWndPreviewCtrl, SCI_SETCODEPAGE, SC_CP_UTF8, 0 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETSIZE, STYLE_DEFAULT, 9 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM) "Courier New" );
	::SendMessage( hWndPreviewCtrl, SCI_SETMARGINWIDTHN, 1, 0 );
	::SendMessage( hWndPreviewCtrl, SCI_USEPOPUP, 0, 0 );

	::SendMessage( hWndPreviewCtrl, SCI_SETLEXER, SCLEX_CPP, 0 );
	::SendMessage( hWndPreviewCtrl, SCI_SETKEYWORDS, 0, (LPARAM) cppKeyWords );
	::SendMessage( hWndPreviewCtrl, SCI_SETKEYWORDS, 1, (LPARAM) cppTypeWords );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_DEFAULT, 0x00000000 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_COMMENT, 0x00008000 ); // /* */
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_COMMENTLINE, 0x00008000 ); // //
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_COMMENTDOC, 0x00808000 ); // /**
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_COMMENTLINEDOC, 0x00808000 ); // ///
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_COMMENTDOCKEYWORD, 0x00808000 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_COMMENTDOCKEYWORDERROR, 0x00808000 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_PREPROCESSORCOMMENT, 0x00008000 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_PREPROCESSOR, 0x00004080 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_NUMBER, 0x000040FF );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_IDENTIFIER, 0x00000000 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_WORD, 0x00FF0000 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETBOLD, SCE_C_WORD, true );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_WORD2, 0x00FF0000 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_OPERATOR, 0x00800000 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETBOLD, SCE_C_OPERATOR, true );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_STRINGEOL, 0x001515A3 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_STRING, 0x000040B0 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_CHARACTER, 0x000040B0 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_UUID, 0x00000000 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_VERBATIM, 0x00000000 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_REGEX, 0x004515A3 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_GLOBALCLASS, 0x00000000 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_STRINGRAW, 0x001515A3 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_TRIPLEVERBATIM, 0x00000000 );
	::SendMessage( hWndPreviewCtrl, SCI_STYLESETFORE, SCE_C_HASHQUOTEDSTRING, 0x00000000 );

	HRSRC hRsrc = ::FindResource( _hInst , MAKEINTRESOURCE(txt_Democode), (LPCTSTR) TEXT("READMEDATA") );
	if( 0 == hRsrc )
	{
		::MessageBox( NULL, TEXT("Error cannot Find \"txt_Democode\" Resource!!!"), TEXT("NppAStyle Message"), MB_ICONSTOP | MB_OK );
		return;
	}

	HGLOBAL hGlobal = ::LoadResource( _hInst, hRsrc );
	if( 0 == hGlobal )
	{
		::MessageBox( NULL, TEXT("Error cannot Load \"txt_Democode\" Resource!!!"), TEXT("NppAStyle Message"), MB_ICONSTOP | MB_OK );
		return;
	}

	m_textPreviewCode = reinterpret_cast<const char *>( ::LockResource( hGlobal ) );
	if( 0 == m_textPreviewCode )
	{
		::MessageBox( NULL, TEXT("Error cannot Lock \"txt_Democode\" Resource!!!"), TEXT("NppAStyle Message"), MB_ICONSTOP | MB_OK );
		return;
	}
}


void NppAStyleOptionDlg::initDlgControl()
{
	SendDlgItemMessage( IDC_CHK_SameAsNppCurView, BM_SETCHECK, m_astyleOption->isSameAsNppCurView, 0 );

	SendDlgItemMessage( IDC_CBB_BRACKET_STYLE, CB_SETCURSEL, ( WPARAM ) m_astyleOption->formattingStyle, 0 );
	SendDlgItemMessage( IDC_CBB_ASTYLE_OPTION_SET, CB_SETCURSEL, ( WPARAM ) m_indexOptionSet, 0 );
	initDlgOptionControl();
	showDlgOptionControl();

	updateDlgTabsetting();

	updateDlgPreviewText();
}


void NppAStyleOptionDlg::initDlgOptionControl()
{
	// Brace Modify Options
	SendDlgItemMessage( IDC_CHK_AttachNamespace, BM_SETCHECK, m_astyleOption->shouldAttachNamespace, 0 );
	SendDlgItemMessage( IDC_CHK_AttachClass, BM_SETCHECK, m_astyleOption->shouldAttachClass, 0 );
	SendDlgItemMessage( IDC_CHK_AttachInline, BM_SETCHECK, m_astyleOption->shouldAttachInline, 0 );
	SendDlgItemMessage( IDC_CHK_AttachExternC, BM_SETCHECK, m_astyleOption->shouldAttachExternC, 0 );
	SendDlgItemMessage( IDC_CHK_AttachClosingWhile, BM_SETCHECK, m_astyleOption->shouldAttachClosingWhile, 0 );
	// Function Declaration Options
	SendDlgItemMessage( IDC_CHK_BreakReturnType, BM_SETCHECK, m_astyleOption->shouldBreakReturnType, 0 );
	SendDlgItemMessage( IDC_CHK_BreakReturnTypeDecl, BM_SETCHECK, m_astyleOption->shouldBreakReturnTypeDecl, 0 );
	// Indentation Options
	SendDlgItemMessage( IDC_CHK_ClassIndent, BM_SETCHECK, m_astyleOption->shouldClassIndent, 0 );
	SendDlgItemMessage( IDC_CHK_ModifierIndent, BM_SETCHECK, m_astyleOption->shouldModifierIndent, 0 );
	SendDlgItemMessage( IDC_CHK_SwitchIndent, BM_SETCHECK, m_astyleOption->shouldSwitchIndent, 0 );
	SendDlgItemMessage( IDC_CHK_CaseIndent, BM_SETCHECK, m_astyleOption->shouldCaseIndent, 0 );
	SendDlgItemMessage( IDC_CHK_NamespaceIndent, BM_SETCHECK, m_astyleOption->shouldNamespaceIndent, 0 );
	SendDlgItemMessage( IDC_CBB_ContinuationIndent, CB_SETCURSEL, ( WPARAM ) m_astyleOption->continuationIndent, 0 );
	SendDlgItemMessage( IDC_CHK_LabelIndent, BM_SETCHECK, m_astyleOption->shouldLabelIndent, 0 );
	SendDlgItemMessage( IDC_CHK_IndentPreprocBlock, BM_SETCHECK, m_astyleOption->shouldIndentPreprocBlock, 0 );
	SendDlgItemMessage( IDC_CHK_IndentPreprocConditional, BM_SETCHECK, m_astyleOption->shouldIndentPreprocConditional, 0 );
	SendDlgItemMessage( IDC_CHK_IndentPreprocDefine, BM_SETCHECK, m_astyleOption->shouldIndentPreprocDefine, 0 );
	SendDlgItemMessage( IDC_CHK_IndentCol1Comments, BM_SETCHECK, m_astyleOption->shouldIndentCol1Comments, 0 );
	SendDlgItemMessage( IDC_CHK_IndentAfterParen, BM_SETCHECK, m_astyleOption->shouldIndentAfterParen, 0 );
	// Padding Options
	SendDlgItemMessage( IDC_CHK_PadOperators, BM_SETCHECK, m_astyleOption->shouldPadOperators, 0 );
	SendDlgItemMessage( IDC_CHK_PadCommas, BM_SETCHECK, m_astyleOption->shouldPadCommas, 0 );
	SendDlgItemMessage( IDC_CHK_PadParensOutside, BM_SETCHECK, m_astyleOption->shouldPadParensOutside, 0 );
	SendDlgItemMessage( IDC_CHK_PadFirstParen, BM_SETCHECK, m_astyleOption->shouldPadFirstParen, 0 );
	SendDlgItemMessage( IDC_CHK_PadParensInside, BM_SETCHECK, m_astyleOption->shouldPadParensInside, 0 );
	SendDlgItemMessage( IDC_CHK_PadHeader, BM_SETCHECK, m_astyleOption->shouldPadHeader, 0 );
	SendDlgItemMessage( IDC_CHK_UnPadParens, BM_SETCHECK, m_astyleOption->shouldUnPadParens, 0 );
	SendDlgItemMessage( IDC_CHK_DeleteEmptyLines, BM_SETCHECK, m_astyleOption->shouldDeleteEmptyLines, 0 );
	SendDlgItemMessage( IDC_CHK_EmptyLineFill, BM_SETCHECK, m_astyleOption->shouldEmptyLineFill, 0 );
	// Formatting Options
	SendDlgItemMessage( IDC_CHK_BreakBlocks, BM_SETCHECK, m_astyleOption->shouldBreakBlocks, 0 );
	SendDlgItemMessage( IDC_CHK_BreakClosingHeaderBlocks, BM_SETCHECK, m_astyleOption->shouldBreakClosingHeaderBlocks, 0 );
	SendDlgItemMessage( IDC_CHK_BreakElseIfs, BM_SETCHECK, m_astyleOption->shouldBreakElseIfs, 0 );
	SendDlgItemMessage( IDC_CHK_AddBraces, BM_SETCHECK, m_astyleOption->shouldAddBraces, 0 );
	SendDlgItemMessage( IDC_CHK_AddOneLineBraces, BM_SETCHECK, m_astyleOption->shouldAddOneLineBraces, 0 );
	SendDlgItemMessage( IDC_CHK_RemoveBraces, BM_SETCHECK, m_astyleOption->shouldRemoveBraces, 0 );
	SendDlgItemMessage( IDC_CHK_BreakOneLineBlocks, BM_SETCHECK, FALSE == m_astyleOption->shouldBreakOneLineBlocks, 0 );
	SendDlgItemMessage( IDC_CHK_BreakOneLineStatements, BM_SETCHECK, FALSE == m_astyleOption->shouldBreakOneLineStatements, 0 );
	SendDlgItemMessage( IDC_CHK_BreakClosingHeaderBraces, BM_SETCHECK, m_astyleOption->shouldBreakClosingHeaderBraces, 0 );
	SendDlgItemMessage( IDC_CHK_BreakOneLineHeaders, BM_SETCHECK, m_astyleOption->shouldBreakOneLineHeaders, 0 );
	//SendDlgItemMessage( IDC_CHK_ConvertTabs, BM_SETCHECK, m_astyleOption->shouldConvertTabs, 0 )
	SendDlgItemMessage( IDC_CHK_CloseTemplates, BM_SETCHECK, m_astyleOption->shouldCloseTemplates, 0 );
	SendDlgItemMessage( IDC_CHK_StripCommentPrefix, BM_SETCHECK, m_astyleOption->shouldStripCommentPrefix, 0 );
	//size_t maxCodeLength;
	SendDlgItemMessage( IDC_CHK_BreakLineAfterLogical, BM_SETCHECK, m_astyleOption->shouldBreakLineAfterLogical, 0 );
	// Objective-C Options
	SendDlgItemMessage( IDC_CHK_ObjC_AlignMethodColon, BM_SETCHECK, m_astyleOption->shouldAlignMethodColon, 0 );
	SendDlgItemMessage( IDC_CHK_ObjC_PadMethodPrefix, BM_SETCHECK, m_astyleOption->shouldPadMethodPrefix, 0 );
	SendDlgItemMessage( IDC_CHK_ObjC_UnPadMethodPrefix, BM_SETCHECK, m_astyleOption->shouldUnPadMethodPrefix, 0 );
	SendDlgItemMessage( IDC_CHK_ObjC_PadReturnType, BM_SETCHECK, m_astyleOption->shouldPadReturnType, 0 );
	SendDlgItemMessage( IDC_CHK_ObjC_UnPadReturnType, BM_SETCHECK, m_astyleOption->shouldUnPadReturnType, 0 );
	SendDlgItemMessage( IDC_CHK_ObjC_PadParamType, BM_SETCHECK, m_astyleOption->shouldPadParamType, 0 );
	SendDlgItemMessage( IDC_CHK_ObjC_UnPadParamType, BM_SETCHECK, m_astyleOption->shouldUnPadParamType, 0 );

	SendDlgItemMessage( IDC_CBB_MinConditionalIndent, CB_SETCURSEL, ( WPARAM ) m_astyleOption->minConditionalOption, 0 );
	SendDlgItemMessage( IDC_CBB_PtrAlign, CB_SETCURSEL, ( WPARAM ) m_astyleOption->pointerAlignment, 0 );
	SendDlgItemMessage( IDC_CBB_RefAlign, CB_SETCURSEL, ( WPARAM ) m_astyleOption->referenceAlignment, 0 );
	SendDlgItemMessage( IDC_CBB_ObjC_ColonPad, CB_SETCURSEL, ( WPARAM ) m_astyleOption->objCColonPadMode, 0 );
	int index = ( m_astyleOption->maxContinuationIndent - 40 ) / 10;
	SendDlgItemMessage( IDC_CBB_MaxContinuationIndent, CB_SETCURSEL, ( WPARAM ) index, 0 );
	index = ( m_astyleOption->maxCodeLength < 50 || m_astyleOption->maxCodeLength > 200 ) ? (int)40 : (int)m_astyleOption->maxCodeLength;
	index = ( index - 40 ) / 10;
	SendDlgItemMessage( IDC_CBB_MaxCodeLength, CB_SETCURSEL, ( WPARAM ) index, 0 );
}

void NppAStyleOptionDlg::showDlgControlRange( const UINT idStart, const UINT idEnd, const bool isShow, const bool isEnable )
{
	for( UINT id = idStart; id <= idEnd; ++id )
	{
		HWND hWndCtrl = GetDlgItem( id );
		if( hWndCtrl != NULL )
		{
			::ShowWindow( hWndCtrl, isShow );
			::EnableWindow( hWndCtrl, isEnable );
		}
	}
}

void NppAStyleOptionDlg::showDlgOptionControl()
{
	bool isEnable = m_astyleOption->formattingStyle != 0;

	// Brace Modify Options
	bool isShow = ( indexBraceModifyOptions == m_indexOptionSet );
	showDlgControlRange( IDC_GRP_Brace_Modify, IDC_CHK_AttachClosingWhile, isShow, isEnable );
	showDlgControlRange( IDC_GRP_Function_Declaration, IDC_CHK_BreakReturnTypeDecl, isShow, isEnable );

	// Indentation Options
	isShow = ( indexIndentationOptions == m_indexOptionSet );
	showDlgControlRange( IDC_GRP_Indentation, IDC_CHK_IndentAfterParen, isShow, isEnable );
	showDlgControlRange( IDC_LBL_ContinuationIndent, IDC_CBB_ContinuationIndent, isShow, isEnable );
	showDlgControlRange( IDC_LBL_MinConditionalIndent, IDC_CBB_MinConditionalIndent, isShow, isEnable );
	showDlgControlRange( IDC_LBL_MaxContinuationIndent, IDC_CBB_MaxContinuationIndent, isShow, isEnable );

	// Padding Options
	isShow = ( indexPaddingOptions == m_indexOptionSet );
	showDlgControlRange( IDC_GRP_Padding, IDC_CHK_PadCommas, isShow, isEnable );
	showDlgControlRange( IDC_LBL_PtrAlign, IDC_CBB_PtrAlign, isShow, isEnable );
	showDlgControlRange( IDC_LBL_RefAlign, IDC_CBB_RefAlign, isShow, isEnable );

	// Formatting Options
	isShow = ( indexFormattingOptions == m_indexOptionSet );
	showDlgControlRange( IDC_GRP_Formatting, IDC_CHK_BreakLineAfterLogical, isShow, isEnable );
	showDlgControlRange( IDC_LBL_MaxCodeLength, IDC_CBB_MaxCodeLength, isShow, isEnable );

	// Objective-C Options
	isShow = ( indexObjectiveCOptions == m_indexOptionSet );
	showDlgControlRange( IDC_GRP_Objective_C, IDC_CHK_ObjC_UnPadParamType, isShow, isEnable );
	showDlgControlRange( IDC_LBL_ObjC_ColonPad, IDC_CBB_ObjC_ColonPad, isShow, isEnable );
}


void NppAStyleOptionDlg::doDialog()
{
	if( !isCreated() )
	{
		updateDlgLangSetting( false );
		m_astyleOption = new NppAStyleOption();
		* m_astyleOption = astyleOptionSet->languageAStyleOption[m_languageMode];
		create( IDD_NPPASTYLE_OPTION_DLG );
	}
	else
	{
		updateDlgLangSetting( true );
		* m_astyleOption = astyleOptionSet->languageAStyleOption[m_languageMode];
		initDlgControl();
	}

	goToCenter();
}


INT_PTR CALLBACK NppAStyleOptionDlg::DlgOptionProc( UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch( Message )
	{
		case WM_COMMAND :
			{
				bool isUpdatePreview = false;
				switch( wParam )
				{
					case IDC_CHK_SameAsNppCurView:
						{
							m_astyleOption->isSameAsNppCurView = BST_CHECKED == SendDlgItemMessage( IDC_CHK_SameAsNppCurView, BM_GETCHECK, 0, 0 );
							updateDlgTabsetting();
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_UseSpace:
						{
							m_astyleOption->isUseSpace = BST_CHECKED == SendDlgItemMessage( IDC_CHK_UseSpace, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					// Brace Modify Options
					case IDC_CHK_AttachNamespace:
						{
							m_astyleOption->shouldAttachNamespace = BST_CHECKED == SendDlgItemMessage( IDC_CHK_AttachNamespace, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_AttachClass:
						{
							m_astyleOption->shouldAttachClass = BST_CHECKED == SendDlgItemMessage( IDC_CHK_AttachClass, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_AttachInline:
						{
							m_astyleOption->shouldAttachInline = BST_CHECKED == SendDlgItemMessage( IDC_CHK_AttachInline, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_AttachExternC:
						{
							m_astyleOption->shouldAttachExternC = BST_CHECKED == SendDlgItemMessage( IDC_CHK_AttachExternC, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_AttachClosingWhile:
						{
							m_astyleOption->shouldAttachClosingWhile = BST_CHECKED == SendDlgItemMessage( IDC_CHK_AttachClosingWhile, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					// Function Declaration Options
					case IDC_CHK_BreakReturnType:
						{
							m_astyleOption->shouldBreakReturnType = BST_CHECKED == SendDlgItemMessage( IDC_CHK_BreakReturnType, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_BreakReturnTypeDecl:
						{
							m_astyleOption->shouldBreakReturnTypeDecl = BST_CHECKED == SendDlgItemMessage( IDC_CHK_BreakReturnTypeDecl, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					// Indentation Options
					case IDC_CHK_ClassIndent:
						{
							m_astyleOption->shouldClassIndent = BST_CHECKED == SendDlgItemMessage( IDC_CHK_ClassIndent, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_ModifierIndent:
						{
							m_astyleOption->shouldModifierIndent = BST_CHECKED == SendDlgItemMessage( IDC_CHK_ModifierIndent, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_SwitchIndent:
						{
							m_astyleOption->shouldSwitchIndent = BST_CHECKED == SendDlgItemMessage( IDC_CHK_SwitchIndent, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_CaseIndent:
						{
							m_astyleOption->shouldCaseIndent = BST_CHECKED == SendDlgItemMessage( IDC_CHK_CaseIndent, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_NamespaceIndent:
						{
							m_astyleOption->shouldNamespaceIndent = BST_CHECKED == SendDlgItemMessage( IDC_CHK_NamespaceIndent, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_IndentAfterParen:
						{
							m_astyleOption->shouldIndentAfterParen = BST_CHECKED == SendDlgItemMessage( IDC_CHK_IndentAfterParen, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_LabelIndent:
						{
							m_astyleOption->shouldLabelIndent = BST_CHECKED == SendDlgItemMessage( IDC_CHK_LabelIndent, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_IndentPreprocBlock:
						{
							m_astyleOption->shouldIndentPreprocBlock = BST_CHECKED == SendDlgItemMessage( IDC_CHK_IndentPreprocBlock, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_IndentPreprocConditional:
						{
							m_astyleOption->shouldIndentPreprocConditional = BST_CHECKED == SendDlgItemMessage( IDC_CHK_IndentPreprocConditional, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_IndentPreprocDefine:
						{
							m_astyleOption->shouldIndentPreprocDefine = BST_CHECKED == SendDlgItemMessage( IDC_CHK_IndentPreprocDefine, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_IndentCol1Comments:
						{
							m_astyleOption->shouldIndentCol1Comments = BST_CHECKED == SendDlgItemMessage( IDC_CHK_IndentCol1Comments, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					//int  maxContinuationIndent;
					// Padding Options
					case IDC_CHK_PadOperators:
						{
							m_astyleOption->shouldPadOperators = BST_CHECKED == SendDlgItemMessage( IDC_CHK_PadOperators, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_PadParensOutside:
						{
							m_astyleOption->shouldPadParensOutside = BST_CHECKED == SendDlgItemMessage( IDC_CHK_PadParensOutside, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_PadFirstParen:
						{
							m_astyleOption->shouldPadFirstParen = BST_CHECKED == SendDlgItemMessage( IDC_CHK_PadFirstParen, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_PadParensInside:
						{
							m_astyleOption->shouldPadParensInside = BST_CHECKED == SendDlgItemMessage( IDC_CHK_PadParensInside, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_PadHeader:
						{
							m_astyleOption->shouldPadHeader = BST_CHECKED == SendDlgItemMessage( IDC_CHK_PadHeader, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_UnPadParens:
						{
							m_astyleOption->shouldUnPadParens = BST_CHECKED == SendDlgItemMessage( IDC_CHK_UnPadParens, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_DeleteEmptyLines:
						{
							m_astyleOption->shouldDeleteEmptyLines = BST_CHECKED == SendDlgItemMessage( IDC_CHK_DeleteEmptyLines, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_PadCommas:
						{
							m_astyleOption->shouldPadCommas = BST_CHECKED == SendDlgItemMessage( IDC_CHK_PadCommas, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_EmptyLineFill:
						{
							m_astyleOption->shouldEmptyLineFill = BST_CHECKED == SendDlgItemMessage( IDC_CHK_EmptyLineFill, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					// Formatting Options
					case IDC_CHK_BreakBlocks:
						{
							m_astyleOption->shouldBreakBlocks = BST_CHECKED == SendDlgItemMessage( IDC_CHK_BreakBlocks, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_BreakClosingHeaderBlocks:
						{
							m_astyleOption->shouldBreakClosingHeaderBlocks = BST_CHECKED == SendDlgItemMessage( IDC_CHK_BreakClosingHeaderBlocks, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_BreakElseIfs:
						{
							m_astyleOption->shouldBreakElseIfs = BST_CHECKED == SendDlgItemMessage( IDC_CHK_BreakElseIfs, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_AddBraces:
						{
							m_astyleOption->shouldAddBraces = BST_CHECKED == SendDlgItemMessage( IDC_CHK_AddBraces, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_AddOneLineBraces:
						{
							m_astyleOption->shouldAddOneLineBraces = BST_CHECKED == SendDlgItemMessage( IDC_CHK_AddOneLineBraces, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_RemoveBraces:
						{
							m_astyleOption->shouldRemoveBraces = BST_CHECKED == SendDlgItemMessage( IDC_CHK_RemoveBraces, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_BreakOneLineBlocks:
						{
							m_astyleOption->shouldBreakOneLineBlocks = BST_CHECKED != SendDlgItemMessage( IDC_CHK_BreakOneLineBlocks, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_BreakOneLineStatements:
						{
							m_astyleOption->shouldBreakOneLineStatements = BST_CHECKED != SendDlgItemMessage( IDC_CHK_BreakOneLineStatements, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_BreakClosingHeaderBraces:
						{
							m_astyleOption->shouldBreakClosingHeaderBraces = BST_CHECKED == SendDlgItemMessage( IDC_CHK_BreakClosingHeaderBraces, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					//bool shouldConvertTabs;
					case IDC_CHK_CloseTemplates:
						{
							m_astyleOption->shouldCloseTemplates = BST_CHECKED == SendDlgItemMessage( IDC_CHK_CloseTemplates, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_StripCommentPrefix:
						{
							m_astyleOption->shouldStripCommentPrefix = BST_CHECKED == SendDlgItemMessage( IDC_CHK_StripCommentPrefix, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					//size_t maxCodeLength;
					case IDC_CHK_BreakLineAfterLogical:
						{
							m_astyleOption->shouldBreakLineAfterLogical = BST_CHECKED == SendDlgItemMessage( IDC_CHK_BreakLineAfterLogical, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_BreakOneLineHeaders:
						{
							m_astyleOption->shouldBreakOneLineHeaders = BST_CHECKED == SendDlgItemMessage( IDC_CHK_BreakOneLineHeaders, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					// Objective-C Options
					case IDC_CHK_ObjC_AlignMethodColon:
						{
							m_astyleOption->shouldAlignMethodColon = BST_CHECKED == SendDlgItemMessage( IDC_CHK_ObjC_AlignMethodColon, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_ObjC_PadMethodPrefix:
						{
							m_astyleOption->shouldPadMethodPrefix = BST_CHECKED == SendDlgItemMessage( IDC_CHK_ObjC_PadMethodPrefix, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_ObjC_UnPadMethodPrefix:
						{
							m_astyleOption->shouldUnPadMethodPrefix = BST_CHECKED == SendDlgItemMessage( IDC_CHK_ObjC_UnPadMethodPrefix, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_ObjC_PadReturnType:
						{
							m_astyleOption->shouldPadReturnType = BST_CHECKED == SendDlgItemMessage( IDC_CHK_ObjC_PadReturnType, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_ObjC_UnPadReturnType:
						{
							m_astyleOption->shouldUnPadReturnType = BST_CHECKED == SendDlgItemMessage( IDC_CHK_ObjC_UnPadReturnType, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_ObjC_PadParamType:
						{
							m_astyleOption->shouldPadParamType = BST_CHECKED == SendDlgItemMessage( IDC_CHK_ObjC_PadParamType, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					case IDC_CHK_ObjC_UnPadParamType:
						{
							m_astyleOption->shouldUnPadParamType = BST_CHECKED == SendDlgItemMessage( IDC_CHK_ObjC_UnPadParamType, BM_GETCHECK, 0, 0 );
							isUpdatePreview = true;
						}
						break;

					default :
						break;
				}

				if( HIWORD( wParam ) == CBN_SELCHANGE )
				{
					int ItemIndex = ::SendMessage( ( HWND ) lParam, ( UINT ) CB_GETCURSEL, 0, 0 );
					switch( LOWORD( wParam ) )
					{
						case IDC_CBB_TABSIZE:
						{
							m_astyleOption->iTabSize = listTabSizeValue[ItemIndex];
							SendDlgItemMessage( IDC_TXT_PREVIEW, SCI_SETTABWIDTH, (WPARAM) m_astyleOption->iTabSize, 0 );
							isUpdatePreview = true;
						}
							break;

						case IDC_CBB_ASTYLE_LANG_MODE:
						{
							m_languageMode = ItemIndex;

							* m_astyleOption = astyleOptionSet->languageAStyleOption[m_languageMode];
							initDlgControl();
							return TRUE;
						}
							break;

						case IDC_CBB_BRACKET_STYLE:
						{
							m_astyleOption->formattingStyle = ItemIndex;
							showDlgOptionControl();
							isUpdatePreview = true;
						}
							break;

						case IDC_CBB_ASTYLE_OPTION_SET:
						{
							m_indexOptionSet = ItemIndex;
							showDlgOptionControl();
							return TRUE;
						}
							break;

						case IDC_CBB_MinConditionalIndent:
						{
							m_astyleOption->minConditionalOption = ItemIndex;
							isUpdatePreview = true;
						}
							break;
						case IDC_CBB_MaxContinuationIndent:
						{
							m_astyleOption->maxContinuationIndent = 10 * ItemIndex + 40 ;
							isUpdatePreview = true;
						}
						case IDC_CBB_MaxCodeLength:
						{
							m_astyleOption->maxCodeLength = ( ItemIndex == 0 || ItemIndex == -1 ) ? std::string::npos : (size_t)( 10 * ItemIndex + 40 ) ;
							isUpdatePreview = true;
						}
							break;
						case IDC_CBB_ContinuationIndent:
						{
							m_astyleOption->continuationIndent = ItemIndex;
							isUpdatePreview = true;
						}
							break;
						case IDC_CBB_PtrAlign:
						{
							m_astyleOption->pointerAlignment = ItemIndex;
							isUpdatePreview = true;
						}
							break;
						case IDC_CBB_RefAlign:
						{
							m_astyleOption->referenceAlignment = ItemIndex;
							isUpdatePreview = true;
						}
							break;
						case IDC_CBB_ObjC_ColonPad:
						{
							m_astyleOption->objCColonPadMode = ItemIndex;
							isUpdatePreview = true;
						}
							break;
						default :
							break;
					}
				}

				if( isUpdatePreview )
				{
					updateDlgPreviewText();
					return TRUE;
				}
			}
			break;
	}
	return FALSE;
}



void NppAStyleOptionDlg::optionImportExport( bool isImport )
{
	OPENFILENAME ofn;       // common dialog box structure
	TCHAR szFileName[MAX_PATH]; // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory( &ofn, sizeof( ofn ) );
	ofn.lStructSize = sizeof( ofn );
	ofn.hwndOwner = _hSelf;
	ofn.lpstrFile = szFileName;
	// Set lpstrFile[0] to '\0' so that GetOpen/SaveFileName does not
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( szFileName );
	ofn.lpstrFilter = TEXT( "Config(*.ini)\0*.ini\0Text(*.txt)\0*.txt\0All(*.*)\0*.*\0" );
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;

	if( isImport )
	{
		ofn.lpstrTitle = TEXT( "Open NppAstyle Config File" );
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Display the Open dialog box.
		if( GetOpenFileName( &ofn ) == TRUE )
		{
			m_astyleOption->loadConfigInfo( astyleOptionSet->languageSectionNames[m_languageMode], ofn.lpstrFile ); // Import current Language AStyleOption
			initDlgControl();
		}
	}
	else
	{
		ofn.lpstrTitle = TEXT( "Save NppAstyle Config File" );
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

		// Display the Save dialog box.
		if( GetSaveFileName( &ofn ) == TRUE )
		{
			m_astyleOption->saveConfigInfo( astyleOptionSet->languageSectionNames[m_languageMode], ofn.lpstrFile ); // Export current Language AStyleOption
		}
	}
}

INT_PTR CALLBACK NppAStyleOptionDlg::run_dlgProc( UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch( Message )
	{
		case WM_INITDIALOG :
		{
			initPreviewCtrl();
			initDlgComboList();
			initDlgControl();
			return TRUE;
		}
			break;

		case WM_COMMAND :
		{
			switch( wParam )
			{
				case IDOK :
				{
					astyleOptionSet->languageAStyleOption[m_languageMode] = * m_astyleOption;
					// m_astyleOption->saveConfigInfo( astyleOptionSet->languageSectionNames[m_languageMode] ); // Save current Language AStyleOption
					astyleOptionSet->saveConfigInfo(); // Save All Language AStyleOption

					display( FALSE );
				}
					return TRUE;

				case IDCANCEL :
				{
					display( FALSE );
				}
					return TRUE;

				case IDC_BTN_INI_IMPORT :
					optionImportExport( true );
					return TRUE;

				case IDC_BTN_INI_EXPORT :
					optionImportExport( false );
					return TRUE;

				default :
					break;
			}
		}
			break;

		default :
			break;
	}
	if( TRUE == DlgOptionProc( Message, wParam, lParam ) )
	{
		return TRUE;
	}
	return FALSE;
}

