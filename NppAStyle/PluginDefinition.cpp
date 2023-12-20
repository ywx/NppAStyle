/***************************************************************
 * Name:        PluginDefinition.cpp
 * Purpose:     PluginDefinition
 * Author:      YWX (wxFortranIndent@163.com)
 * Created:     2015-7-25
 * Copyright:   (c) YWX <wxFortranIndent@163.com>
 * Licence:     GNU General Public License, version 3
 **************************************************************/

//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <stdio.h>
#include <Shlwapi.h>
#include "PluginDefinition.h"
#include "menuCmdID.h"
#include "AboutDlg.h"
#include "NppAStyleOptionDlg.h"
#include "resource.h"
#include "astyle_main.h"
#include <sstream>
#include "NppAStyleOption.h"

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"AStyleLib.lib")

// The plugin data that Notepad++ needs
FuncItem funcItem[nbFunc];

// The data of Notepad++ that you can use in your plugin commands
NppData nppData;
HINSTANCE hModule;

// dialog
AboutDlg aboutDlg;
AboutDlg readmeDlg( true );
NppAStyleOptionDlg astyleOptionDlg;

// option data
TCHAR NppAStyleConfigFilePath[MAX_PATH];
NppAStyleOptionSet astyleOptionSet;


TCHAR *initNppAStyleConfigFilePath( bool isInit )
{
	if( isInit == false )
	{
		return( NppAStyleConfigFilePath );
	}

	::SendMessage( nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, ( LPARAM )NppAStyleConfigFilePath );
	if( ::PathFileExists( NppAStyleConfigFilePath ) == FALSE )
	{
		::CreateDirectory( NppAStyleConfigFilePath, NULL );
	}

	_sntprintf( NppAStyleConfigFilePath, MAX_PATH, TEXT( "%s\\%s.ini" ), NppAStyleConfigFilePath, NPP_PLUGIN_NAME );

	return( NppAStyleConfigFilePath );
}



// Initialize your plugin data here
// It will be called while plugin loading
void pluginInit( HANDLE hModule )
{
	::hModule = ( HINSTANCE )hModule;
}


//Clean up your plugin commands allocation (if any)
void commandMenuCleanUp();

// Here you can do the clean up, save the parameters (if any) for the next session
void pluginCleanUp()
{
	commandMenuCleanUp();
}


void formatCode();
void openOptionDlg();
void openAboutDlg();
void openReadMeDlg();

// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{
	//--------------------------------------------//
	//-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
	//--------------------------------------------//
	// with function :
	// setCommand(int index,                      // zero based number to indicate the order of command
	//            TCHAR *commandName,             // the command name that you want to see in plugin menu
	//            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
	//            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
	//            bool check0nInit                // optional. Make this menu item be checked visually
	//            );

	astyleOptionSet.loadConfigInfo();

	// Shortcut :
	// Following code makes the first command
	// bind to the shortcut Alt-F
	ShortcutKey *shKey = new ShortcutKey;
	shKey->_isAlt = true;
	shKey->_isCtrl = false;
	shKey->_isShift = false;
	shKey->_key = 0x5A; //VK_Z

	setCommand( 0, TEXT( "Format Code" ), formatCode, shKey, false );
	setCommand( 1, TEXT( "Options ..." ), openOptionDlg, NULL, false );
	setCommand( 2, TEXT( "---" ), NULL, NULL, false );
	setCommand( 3, TEXT( "ReadMe ..." ), openReadMeDlg, NULL, false );
	setCommand( 4, TEXT( "About ..." ), openAboutDlg, NULL, false );

	aboutDlg.init( ( HINSTANCE )hModule, nppData );
	readmeDlg.init( ( HINSTANCE )hModule, nppData );
	astyleOptionDlg.init( ( HINSTANCE )hModule, nppData, & astyleOptionSet );
}


// Here you can do the clean up (especially for the shortcut)
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
	delete funcItem[0]._pShKey;
}


// This function help you to initialize your plugin commands
bool setCommand( size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit )
{
	if( index >= nbFunc )
		return false;

	if( !pFunc )
		return false;

	lstrcpy( funcItem[index]._itemName, cmdName );
	funcItem[index]._pFunc = pFunc;
	funcItem[index]._init2Check = check0nInit;
	funcItem[index]._pShKey = sk;

	return true;
}


//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//

HWND getNppCurrentScintilla()
{
	int which = 0;
	::SendMessage( nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, ( LPARAM )&which );
	return( ( which == 0 ) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle );
}

int getNppVersion()
{
	return ::SendMessage( nppData._nppHandle, NPPM_GETNPPVERSION, TRUE, 0 );
}

int getNppCurrentLangId()
{
	int langType = 0;
	::SendMessage( nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, ( LPARAM )&langType );
	return langType;
}

static void formatRunProcCallback( const char *in, const char *out, HWND hwin )
{
	if( 0 != strcmp( in, out ) )
		::SendMessage( hwin, SCI_SETTEXT, 0, ( LPARAM ) out );
}

void AStyleCode( const char *textBuffer, const NppAStyleOption &m_astyleOption, fpRunProc fpRunProcHandler, HWND hwin )
{
	astyle::ASFormatter formatter;

	m_astyleOption.setFormatterOption( formatter );

	// 0 C, 1 C++, 2 Java, 3 C#, 4 Objective-C, 5 JavaScript
	if( m_astyleOption.languageMode == 0 || m_astyleOption.languageMode == 1 )
	{
		formatter.setCStyle();
		formatter.setModeManuallySet( true );
	}
	else if( m_astyleOption.languageMode == 2 )
	{
		formatter.setJavaStyle();
		formatter.setModeManuallySet( true );
	}
	else if( m_astyleOption.languageMode == 3 )
	{
		formatter.setSharpStyle();
		formatter.setModeManuallySet( true );
	}
	else if( m_astyleOption.languageMode == 4 )
	{
		formatter.setObjCStyle();
		formatter.setModeManuallySet( true );
	}
	else if( m_astyleOption.languageMode == 5 )
	{
		formatter.setJSStyle();
		formatter.setModeManuallySet( true );
	}

	if( m_astyleOption.isSameAsNppCurView )
	{
		bool isUseSpace = false;
		int iTabSize = 4;
		HWND curScintilla = getNppCurrentScintilla();

		isUseSpace = 0 == ::SendMessage( curScintilla, SCI_GETUSETABS, 0, 0 );
		iTabSize = ::SendMessage( curScintilla, SCI_GETTABWIDTH, 0, 0 );

		if( isUseSpace == false )
		{
			formatter.setTabIndentation( iTabSize, false );
		}
		else
		{
			formatter.setSpaceIndentation( iTabSize );
		}
	}

	std::stringstream in( textBuffer );
	astyle::ASStreamIterator<std::stringstream> streamIterator( &in );
	std::ostringstream out;
	formatter.init( &streamIterator );

	while( formatter.hasMoreLines() )
	{
		out << formatter.nextLine();
		if( formatter.hasMoreLines() )
			out << streamIterator.getOutputEOL();
		else
		{
			// this can happen if the file if missing a closing bracket and break-blocks is requested
			if( formatter.getIsLineReady() )
			{
				out << streamIterator.getOutputEOL();
				out << formatter.nextLine();
			}
		}
	}

	fpRunProcHandler( textBuffer, out.str().c_str(), hwin );
}


void formatCode()
{
	int langType;
	int languageMode = 0;
	::SendMessage( nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, ( LPARAM )&langType );

	if( langType == L_C )
	{
		languageMode = 0;
	}
	else if( langType == L_CPP )
	{
		languageMode = 1;
	}
	else if( langType == L_JAVA )
	{
		languageMode = 2;
	}
	else if( langType == L_CS )
	{
		languageMode = 3;
	}
	else if( langType == L_OBJC )
	{
		languageMode = 4;
	}
	else if( langType == L_JAVASCRIPT )
	{
		languageMode = 5;
	}
	else
	{
		::MessageBox( NULL, TEXT( "AStyle only support C, C++, Objective-C, C# and Java" ), TEXT( "NppAStyle Message" ), 0 );
		return;
	}

	if( astyleOptionSet.languageAStyleOption[languageMode].formattingStyle == 0 )
		return;

	HWND curScintilla = getNppCurrentScintilla();

	bool isReadOnly = 0 != ::SendMessage( curScintilla, SCI_GETREADONLY, 0, 0 );
	if( isReadOnly )
	{
		::MessageBox( NULL, TEXT( "Current document is read-only !" ), TEXT( "NppAStyle Message" ), 0 );
		return;
	}

	int textSize = ( int )::SendMessage( curScintilla, SCI_GETLENGTH, 0, 0 );
	textSize += 1;
	char *textBuffer = ( char * )::malloc( textSize );
	::SendMessage( curScintilla, SCI_GETTEXT, ( WPARAM )textSize, ( LPARAM )textBuffer );

	const unsigned int pos_cur = ::SendMessage( curScintilla, SCI_GETCURRENTPOS, 0, 0 );
	const unsigned int lineNumber_cur = ::SendMessage( curScintilla, SCI_LINEFROMPOSITION, pos_cur, 0 );

	AStyleCode( textBuffer, astyleOptionSet.languageAStyleOption[languageMode], formatRunProcCallback, curScintilla );

	::SendMessage( curScintilla, SCI_GOTOLINE, lineNumber_cur, 0 );

	::free( textBuffer );
}

void openOptionDlg()
{
	astyleOptionDlg.doDialog();
}

void openReadMeDlg()
{
	readmeDlg.doDialog();
}

void openAboutDlg()
{
	aboutDlg.doDialog();
}
