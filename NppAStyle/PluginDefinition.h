/***************************************************************
 * Name:        PluginDefinition.h
 * Purpose:     declaration of PluginDefinition
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

#ifndef _PLUGINDEFINITION_H_
#define _PLUGINDEFINITION_H_

// All difinitions of plugin interface
#include "PluginInterface.h"

//-------------------------------------//
//-- STEP 1. DEFINE YOUR PLUGIN NAME --//
//-------------------------------------//
// Here define your plugin name
const TCHAR NPP_PLUGIN_NAME[] = TEXT( "NppAStyle" );

//-----------------------------------------------//
//-- STEP 2. DEFINE YOUR PLUGIN COMMAND NUMBER --//
//-----------------------------------------------//
// Here define the number of your plugin commands
const int nbFunc = 5;


// Initialization of your plugin data
// It will be called while plugin loading
void pluginInit( HANDLE hModule );

// Cleaning of your plugin
// It will be called while plugin unloading
void pluginCleanUp();

//Initialization of your plugin commands
void commandMenuInit();

// Function which sets your command
bool setCommand( size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk = NULL, bool check0nInit = false );


// Your plugin command functions

// define pointers to callback proc formatter code
typedef void ( * fpRunProc )( const char *in, const char *out, HWND hwin );

struct NppAStyleOption;

void AStyleCode( const char *textBuffer, const NppAStyleOption &m_astyleOption, fpRunProc fpRunProcHandler, HWND hwin );

#endif // _PLUGINDEFINITION_H_
