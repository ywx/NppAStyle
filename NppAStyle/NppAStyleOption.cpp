/***************************************************************
 * Name:        NppAStyleOption..cpp
 * Purpose:     Struct NppAStyleOption
 * Author:      YWX (wxFortranIndent@163.com)
 * Created:     2015-7-26
 * Copyright:   (c) YWX <wxFortranIndent@163.com>
 * Licence:     GNU General Public License, version 3
 **************************************************************/

#include <Windows.h>
#include <tchar.h>
#include "astyle.h"
#include "NppAStyleOption.h"

extern TCHAR *initNppAStyleConfigFilePath( bool isInit );

#define bool2TEXT(v) v ? TEXT( "1" ) : TEXT( "0" )

#define keySectionName TEXT( "NppAStyle" )
// Tab Options
#define keySameAsNppCurView TEXT( "SameAsNppCurView" )
#define keyUseSpace TEXT( "UseSpace" )
#define keyTabSize TEXT( "TabSize" )

#define keyCurBracketStyle TEXT( "BracketStyle" )
#define keyPointerAlignment TEXT( "PointerAlignment" )
#define keyReferenceAlignment TEXT( "ReferenceAlignment" )
#define keyLineEnd TEXT( "LineEnd" )
// Bracket Modify Options
#define keyAttachNamespace TEXT( "AttachNamespace" )
#define keyAttachClass TEXT( "AttachClass" )
#define keyAttachInline TEXT( "AttachInline" )
#define keyAttachExternC TEXT( "AttachExternC" )
// Indentation Options
#define keyClassIndent TEXT( "ClassIndent" )
#define keyModifierIndent TEXT( "ModifierIndent" )
#define keySwitchIndent TEXT( "SwitchIndent" )
#define keyCaseIndent TEXT( "CaseIndent" )
#define keyNamespaceIndent TEXT( "NamespaceIndent" )
#define keyLabelIndent TEXT( "LabelIndent" )
#define keyIndentPreprocBlock TEXT( "IndentPreprocBlock" )
#define keyIndentPreprocConditional TEXT( "IndentPreprocConditional" )
#define keyIndentPreprocDefine TEXT( "IndentPreprocDefine" )
#define keyIndentCol1Comments TEXT( "IndentCol1Comments" )
#define keyMinConditionalOption TEXT( "MinConditionalOption" )
#define keyMaxInStatementIndent TEXT( "MaxInStatementIndent" )
// Padding Options
#define keyPadOperators TEXT( "PadOperators" )
#define keyPadParensOutside TEXT( "PadParensOutside" )
#define keyPadFirstParen TEXT( "PadFirstParen" )
#define keyPadParensInside TEXT( "PadParensInside" )
#define keyPadHeader TEXT( "PadHeader" )
#define keyUnPadParens TEXT( "UnPadParens" )
#define keyDeleteEmptyLines TEXT( "DeleteEmptyLines" )
#define keyEmptyLineFill TEXT( "EmptyLineFill" )
// Formatting Options
#define keyBreakBlocks TEXT( "BreakBlocks" )
#define keyBreakClosingHeaderBlocks TEXT( "BreakClosingHeaderBlocks" )
#define keyBreakElseIfs TEXT( "BreakElseIfs" )
#define keyAddBrackets TEXT( "AddBrackets" )
#define keyAddOneLineBrackets TEXT( "AddOneLineBrackets" )
#define keyRemoveBrackets TEXT( "RemoveBrackets" )
#define keyBreakOneLineBlocks TEXT( "BreakOneLineBlocks" )
#define keyBreakOneLineStatements TEXT( "BreakOneLineStatements" )
#define keyBreakClosingHeaderBrackets TEXT( "BreakClosingHeaderBrackets" )
#define keyConvertTabs TEXT( "ConvertTabs" )
#define keyCloseTemplates TEXT( "CloseTemplates" )
#define keyStripCommentPrefix TEXT( "StripCommentPrefix" )
#define keyMaxCodeLength TEXT( "MaxCodeLength" )
#define keyBreakLineAfterLogical TEXT( "BreakLineAfterLogical" )
// Objective-C Options
#define keyAlignMethodColon TEXT( "AlignMethodColon" )
#define keyPadMethodPrefix TEXT( "PadMethodPrefix" )
#define keyUnPadMethodPrefix TEXT( "UnPadMethodPrefix" )
#define keyObjCColonPadMode TEXT( "ObjCColonPadMode" )

void NppAStyleOption::reset()
{
	// Tab Options
	isSameAsNppCurView = true;
	isUseSpace = false;
	iTabSize = 4;

	formattingStyle = astyle::STYLE_NONE;
	pointerAlignment = astyle::PTR_ALIGN_NONE;
	referenceAlignment = astyle::REF_SAME_AS_PTR;
	//lineEnd = astyle::LINEEND_DEFAULT;
	languageMode = 0; // 0 default, 1 C, 2 Java, 3 C#

	// Bracket Modify Options
	shouldAttachNamespace = false;
	shouldAttachClass = false;
	shouldAttachInline = false;
	shouldAttachExternC = false;
	// Indentation Options
	shouldClassIndent = false;
	shouldModifierIndent = false;
	shouldSwitchIndent = false;
	shouldCaseIndent = false;
	shouldNamespaceIndent = false;
	shouldLabelIndent = false;
	shouldIndentPreprocBlock = false;
	shouldIndentPreprocConditional = false;
	shouldIndentPreprocDefine = false;
	shouldIndentCol1Comments = false;
	minConditionalOption = astyle::MINCOND_TWO;
	maxInStatementIndent = 40;
	// Padding Options
	shouldPadOperators = false;
	shouldPadParensOutside = false;
	shouldPadFirstParen = false;
	shouldPadParensInside = false;
	shouldPadHeader = false;
	shouldUnPadParens = false;
	shouldDeleteEmptyLines = false;
	shouldEmptyLineFill = false;
	// Formatting Options
	shouldBreakBlocks = false;
	shouldBreakClosingHeaderBlocks = false;
	shouldBreakElseIfs = false;
	shouldAddBrackets = false;
	shouldAddOneLineBrackets = false;
	shouldRemoveBrackets = false;
	shouldBreakOneLineBlocks = true;
	shouldBreakOneLineStatements = true;
	shouldBreakClosingHeaderBrackets = false;
	//shouldConvertTabs = false;
	shouldCloseTemplates = false;
	shouldStripCommentPrefix = false;
	maxCodeLength = std::string::npos;
	shouldBreakLineAfterLogical = false;
	// Objective-C Options
	shouldAlignMethodColon = false;
	shouldPadMethodPrefix = false;
	shouldUnPadMethodPrefix = false;
	objCColonPadMode = astyle::COLON_PAD_NO_CHANGE;
}

void NppAStyleOption::setFormatterOption( astyle::ASFormatter &formatter ) const
{
	formatter.setFormattingStyle( astyle::FormatStyle( formattingStyle ) );
	if( isUseSpace == false )
	{
		formatter.setTabIndentation( iTabSize, false );
	}
	else
	{
		formatter.setSpaceIndentation( iTabSize );
	}

	formatter.setPointerAlignment( astyle::PointerAlign( pointerAlignment ) );
	formatter.setReferenceAlignment( astyle::ReferenceAlign( referenceAlignment ) );
	//formatter.setLineEndFormat( astyle::LineEndFormat( lineEnd ) );

	// Bracket Modify Options
	formatter.setAttachNamespace( shouldAttachNamespace );
	formatter.setAttachClass( shouldAttachClass );
	formatter.setAttachInline( shouldAttachInline );
	formatter.setAttachExternC( shouldAttachExternC );
	// Indentation Options
	formatter.setClassIndent( shouldClassIndent );
	formatter.setModifierIndent( shouldModifierIndent );
	formatter.setSwitchIndent( shouldSwitchIndent );
	formatter.setCaseIndent( shouldCaseIndent );
	formatter.setNamespaceIndent( shouldNamespaceIndent );
	formatter.setLabelIndent( shouldLabelIndent );
	formatter.setPreprocBlockIndent( shouldIndentPreprocBlock );
	formatter.setPreprocConditionalIndent( shouldIndentPreprocConditional );
	formatter.setPreprocDefineIndent( shouldIndentPreprocDefine );
	formatter.setIndentCol1CommentsMode( shouldIndentCol1Comments );
	formatter.setMinConditionalIndentOption( minConditionalOption );
	formatter.setMaxInStatementIndentLength( maxInStatementIndent );
	// Padding Options
	formatter.setOperatorPaddingMode( shouldPadOperators );
	formatter.setParensOutsidePaddingMode( shouldPadParensOutside );
	formatter.setParensFirstPaddingMode( shouldPadFirstParen );
	formatter.setParensInsidePaddingMode( shouldPadParensInside );
	formatter.setParensHeaderPaddingMode( shouldPadHeader );
	formatter.setParensUnPaddingMode( shouldUnPadParens );
	formatter.setDeleteEmptyLinesMode( shouldDeleteEmptyLines );
	formatter.setEmptyLineFill( shouldEmptyLineFill );
	// Formatting Options
	formatter.setBreakBlocksMode( shouldBreakBlocks );
	formatter.setBreakClosingHeaderBlocksMode( shouldBreakClosingHeaderBlocks );
	formatter.setBreakElseIfsMode( shouldBreakElseIfs );
	formatter.setAddBracketsMode( shouldAddBrackets );
	formatter.setAddOneLineBracketsMode( shouldAddOneLineBrackets );
	formatter.setRemoveBracketsMode( shouldRemoveBrackets );
	formatter.setBreakOneLineBlocksMode( shouldBreakOneLineBlocks );
	formatter.setSingleStatementsMode( shouldBreakOneLineStatements );
	formatter.setBreakClosingHeaderBracketsMode( shouldBreakClosingHeaderBrackets );
	//formatter.setTabSpaceConversionMode( shouldConvertTabs );
	formatter.setCloseTemplatesMode( shouldCloseTemplates );
	formatter.setStripCommentPrefix( shouldStripCommentPrefix );
	formatter.setMaxCodeLength( maxCodeLength ); // <= 200
	formatter.setBreakAfterMode( shouldBreakLineAfterLogical );
	// Objective-C Options
	formatter.setAlignMethodColon( shouldAlignMethodColon );
	formatter.setMethodPrefixPaddingMode( shouldPadMethodPrefix );
	formatter.setMethodPrefixUnPaddingMode( shouldUnPadMethodPrefix );
	formatter.setObjCColonPaddingMode( astyle::ObjCColonPad( objCColonPadMode ) );
}

void NppAStyleOption::loadConfigInfo( const TCHAR strFilePath[] )
{
	const TCHAR *iniFilePath;
	if( strFilePath == NULL )
	{
		iniFilePath = initNppAStyleConfigFilePath( true );
	}
	else
	{
		iniFilePath = strFilePath;
	}

	formattingStyle = ::GetPrivateProfileInt( keySectionName, keyCurBracketStyle, formattingStyle, iniFilePath );
	formattingStyle = ( formattingStyle < astyle::STYLE_NONE || formattingStyle > astyle::STYLE_LISP ) ? astyle::STYLE_NONE : formattingStyle;
	isSameAsNppCurView = 0 != ::GetPrivateProfileInt( keySectionName, keySameAsNppCurView, isSameAsNppCurView, iniFilePath );
	isUseSpace = 0 != ::GetPrivateProfileInt( keySectionName, keyUseSpace, isUseSpace, iniFilePath );
	iTabSize = ::GetPrivateProfileInt( keySectionName, keyTabSize, iTabSize, iniFilePath );
	iTabSize = ( iTabSize < 2 || iTabSize > 20 ) ? 4 : iTabSize; // default 4

	pointerAlignment = ::GetPrivateProfileInt( keySectionName, keyPointerAlignment, pointerAlignment, iniFilePath );
	pointerAlignment = ( pointerAlignment < astyle::PTR_ALIGN_NONE || pointerAlignment > astyle::PTR_ALIGN_NAME ) ? astyle::PTR_ALIGN_NONE : pointerAlignment;
	referenceAlignment = ::GetPrivateProfileInt( keySectionName, keyReferenceAlignment, referenceAlignment, iniFilePath );
	referenceAlignment = ( referenceAlignment < astyle::REF_ALIGN_NONE || referenceAlignment > astyle::REF_SAME_AS_PTR ) ? astyle::REF_ALIGN_NONE : referenceAlignment;
	//lineEnd = ::GetPrivateProfileInt( keySectionName, keyLineEnd, lineEnd, iniFilePath );
	//lineEnd  = ( lineEnd < astyle::LINEEND_DEFAULT || lineEnd > astyle::LINEEND_MACOLD ) ? astyle::LINEEND_DEFAULT : lineEnd;

	// Bracket Modify Options
	shouldAttachNamespace = 0 != ::GetPrivateProfileInt( keySectionName, keyAttachNamespace, shouldAttachNamespace, iniFilePath );
	shouldAttachClass = 0 != ::GetPrivateProfileInt( keySectionName, keyAttachClass, shouldAttachClass, iniFilePath );
	shouldAttachInline = 0 != ::GetPrivateProfileInt( keySectionName, keyAttachInline, shouldAttachInline, iniFilePath );
	shouldAttachExternC = 0 != ::GetPrivateProfileInt( keySectionName, keyAttachExternC, shouldAttachExternC, iniFilePath );
	// Indentation Options
	shouldClassIndent = 0 != ::GetPrivateProfileInt( keySectionName, keyClassIndent, shouldClassIndent, iniFilePath );
	shouldModifierIndent = 0 != ::GetPrivateProfileInt( keySectionName, keyModifierIndent, shouldModifierIndent, iniFilePath );
	shouldSwitchIndent = 0 != ::GetPrivateProfileInt( keySectionName, keySwitchIndent, shouldSwitchIndent, iniFilePath );
	shouldCaseIndent = 0 != ::GetPrivateProfileInt( keySectionName, keyCaseIndent, shouldCaseIndent, iniFilePath );
	shouldNamespaceIndent = 0 != ::GetPrivateProfileInt( keySectionName, keyNamespaceIndent, shouldNamespaceIndent, iniFilePath );
	shouldLabelIndent = 0 != ::GetPrivateProfileInt( keySectionName, keyLabelIndent, shouldLabelIndent, iniFilePath );
	shouldIndentPreprocBlock = 0 != ::GetPrivateProfileInt( keySectionName, keyIndentPreprocBlock, shouldIndentPreprocBlock, iniFilePath );
	shouldIndentPreprocConditional = 0 != ::GetPrivateProfileInt( keySectionName, keyIndentPreprocConditional, shouldIndentPreprocConditional, iniFilePath );
	shouldIndentPreprocDefine = 0 != ::GetPrivateProfileInt( keySectionName, keyIndentPreprocDefine, shouldIndentPreprocDefine, iniFilePath );
	shouldIndentCol1Comments = 0 != ::GetPrivateProfileInt( keySectionName, keyIndentCol1Comments, shouldIndentCol1Comments, iniFilePath );
	minConditionalOption = ::GetPrivateProfileInt( keySectionName, keyMinConditionalOption, minConditionalOption, iniFilePath );
	minConditionalOption  = ( minConditionalOption < astyle::MINCOND_ZERO || minConditionalOption > astyle::MINCOND_ONEHALF ) ? astyle::MINCOND_TWO : minConditionalOption;
	maxInStatementIndent = ::GetPrivateProfileInt( keySectionName, keyMaxInStatementIndent, maxInStatementIndent, iniFilePath );
	maxInStatementIndent  = ( maxInStatementIndent < 40 || maxInStatementIndent > 120 ) ? 40 : maxInStatementIndent;
	// Padding Options
	shouldPadOperators = 0 != ::GetPrivateProfileInt( keySectionName, keyPadOperators, shouldPadOperators, iniFilePath );
	shouldPadParensOutside = 0 != ::GetPrivateProfileInt( keySectionName, keyPadParensOutside, shouldPadParensOutside, iniFilePath );
	shouldPadFirstParen = 0 != ::GetPrivateProfileInt( keySectionName, keyPadFirstParen, shouldPadFirstParen, iniFilePath );
	shouldPadParensInside = 0 != ::GetPrivateProfileInt( keySectionName, keyPadParensInside, shouldPadParensInside, iniFilePath );
	shouldPadHeader = 0 != ::GetPrivateProfileInt( keySectionName, keyPadHeader, shouldPadHeader, iniFilePath );
	shouldUnPadParens = 0 != ::GetPrivateProfileInt( keySectionName, keyUnPadParens, shouldUnPadParens, iniFilePath );
	shouldDeleteEmptyLines = 0 != ::GetPrivateProfileInt( keySectionName, keyDeleteEmptyLines, shouldDeleteEmptyLines, iniFilePath );
	shouldEmptyLineFill = 0 != ::GetPrivateProfileInt( keySectionName, keyEmptyLineFill, shouldEmptyLineFill, iniFilePath );
	// Formatting Options
	shouldBreakBlocks = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakBlocks, shouldBreakBlocks, iniFilePath );
	shouldBreakClosingHeaderBlocks = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakClosingHeaderBlocks, shouldBreakClosingHeaderBlocks, iniFilePath );
	shouldBreakElseIfs = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakElseIfs, shouldBreakElseIfs, iniFilePath );
	shouldAddBrackets = 0 != ::GetPrivateProfileInt( keySectionName, keyAddBrackets, shouldAddBrackets, iniFilePath );
	shouldAddOneLineBrackets = 0 != ::GetPrivateProfileInt( keySectionName, keyAddOneLineBrackets, shouldAddOneLineBrackets, iniFilePath );
	shouldRemoveBrackets = 0 != ::GetPrivateProfileInt( keySectionName, keyRemoveBrackets, shouldRemoveBrackets, iniFilePath );
	shouldBreakOneLineBlocks = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakOneLineBlocks, shouldBreakOneLineBlocks, iniFilePath );
	shouldBreakOneLineStatements = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakOneLineStatements, shouldBreakOneLineStatements, iniFilePath );
	shouldBreakClosingHeaderBrackets = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakClosingHeaderBrackets, shouldBreakClosingHeaderBrackets, iniFilePath );
	//shouldConvertTabs = 0 != ::GetPrivateProfileInt( keySectionName, keyConvertTabs, shouldConvertTabs, iniFilePath );
	shouldCloseTemplates = 0 != ::GetPrivateProfileInt( keySectionName, keyCloseTemplates, shouldCloseTemplates, iniFilePath );
	shouldStripCommentPrefix = 0 != ::GetPrivateProfileInt( keySectionName, keyStripCommentPrefix, shouldStripCommentPrefix, iniFilePath );
	maxCodeLength = ::GetPrivateProfileInt( keySectionName, keyMaxCodeLength, maxCodeLength, iniFilePath );
	maxCodeLength = ( maxCodeLength < 50 || maxCodeLength > 200 ) ? std::string::npos : maxCodeLength;
	shouldBreakLineAfterLogical = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakLineAfterLogical, shouldBreakLineAfterLogical, iniFilePath );
	// Objective-C Options
	shouldAlignMethodColon = 0 != ::GetPrivateProfileInt( keySectionName, keyAlignMethodColon, shouldAlignMethodColon, iniFilePath );
	shouldPadMethodPrefix = 0 != ::GetPrivateProfileInt( keySectionName, keyPadMethodPrefix, shouldPadMethodPrefix, iniFilePath );
	shouldUnPadMethodPrefix = 0 != ::GetPrivateProfileInt( keySectionName, keyUnPadMethodPrefix, shouldUnPadMethodPrefix, iniFilePath );
	objCColonPadMode = ::GetPrivateProfileInt( keySectionName, keyObjCColonPadMode, objCColonPadMode, iniFilePath );
	objCColonPadMode = ( objCColonPadMode < astyle::COLON_PAD_NO_CHANGE || objCColonPadMode > astyle::COLON_PAD_BEFORE ) ? astyle::COLON_PAD_NO_CHANGE : objCColonPadMode;
}

void NppAStyleOption::saveConfigInfo( const TCHAR strFilePath[] ) const
{
	const TCHAR *iniFilePath;
	if( strFilePath == NULL )
	{
		iniFilePath = initNppAStyleConfigFilePath( false );
	}
	else
	{
		iniFilePath = strFilePath;
	}

	TCHAR buffer[16];

	_itot( formattingStyle, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyCurBracketStyle, buffer, iniFilePath );

	_itot( iTabSize, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyTabSize, buffer, iniFilePath );

	::WritePrivateProfileString( keySectionName, keyUseSpace, bool2TEXT( isUseSpace ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keySameAsNppCurView, bool2TEXT( isSameAsNppCurView ), iniFilePath );

	_itot( pointerAlignment, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyPointerAlignment, buffer, iniFilePath );
	_itot( referenceAlignment, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyReferenceAlignment, buffer, iniFilePath );
	//_itot( lineEnd, buffer, 10 );
	//::WritePrivateProfileString( keySectionName, keyLineEnd, buffer, iniFilePath );

	// Bracket Modify Options
	::WritePrivateProfileString( keySectionName, keyAttachNamespace, bool2TEXT( shouldAttachNamespace ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyAttachClass, bool2TEXT( shouldAttachClass ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyAttachInline, bool2TEXT( shouldAttachInline ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyAttachExternC, bool2TEXT( shouldAttachExternC ), iniFilePath );
	// Indentation Options
	::WritePrivateProfileString( keySectionName, keyClassIndent, bool2TEXT( shouldClassIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyModifierIndent, bool2TEXT( shouldModifierIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keySwitchIndent, bool2TEXT( shouldSwitchIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyCaseIndent, bool2TEXT( shouldCaseIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyNamespaceIndent, bool2TEXT( shouldNamespaceIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyLabelIndent, bool2TEXT( shouldLabelIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyIndentPreprocBlock, bool2TEXT( shouldIndentPreprocBlock ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyIndentPreprocConditional, bool2TEXT( shouldIndentPreprocConditional ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyIndentPreprocDefine, bool2TEXT( shouldIndentPreprocDefine ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyIndentCol1Comments, bool2TEXT( shouldIndentCol1Comments ), iniFilePath );
	_itot( minConditionalOption, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyMinConditionalOption, buffer, iniFilePath );
	_itot( maxInStatementIndent, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyMaxInStatementIndent, buffer, iniFilePath );
	// Padding Options
	::WritePrivateProfileString( keySectionName, keyPadOperators, bool2TEXT( shouldPadOperators ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyPadParensOutside, bool2TEXT( shouldPadParensOutside ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyPadFirstParen, bool2TEXT( shouldPadFirstParen ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyPadParensInside, bool2TEXT( shouldPadParensInside ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyPadHeader, bool2TEXT( shouldPadHeader ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyUnPadParens, bool2TEXT( shouldUnPadParens ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyDeleteEmptyLines, bool2TEXT( shouldDeleteEmptyLines ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyEmptyLineFill, bool2TEXT( shouldEmptyLineFill ), iniFilePath );
	// Formatting Options
	::WritePrivateProfileString( keySectionName, keyBreakBlocks, bool2TEXT( shouldBreakBlocks ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakClosingHeaderBlocks, bool2TEXT( shouldBreakClosingHeaderBlocks ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakElseIfs, bool2TEXT( shouldBreakElseIfs ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyAddBrackets, bool2TEXT( shouldAddBrackets ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyAddOneLineBrackets, bool2TEXT( shouldAddOneLineBrackets ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyRemoveBrackets, bool2TEXT( shouldRemoveBrackets ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakOneLineBlocks, bool2TEXT( shouldBreakOneLineBlocks ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakOneLineStatements, bool2TEXT( shouldBreakOneLineStatements ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakClosingHeaderBrackets, bool2TEXT( shouldBreakClosingHeaderBrackets ), iniFilePath );
	//::WritePrivateProfileString( keySectionName, keyConvertTabs, bool2TEXT( shouldConvertTabs ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyCloseTemplates, bool2TEXT( shouldCloseTemplates ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyStripCommentPrefix, bool2TEXT( shouldStripCommentPrefix ), iniFilePath );
	_itot( maxCodeLength, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyMaxCodeLength, buffer, iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakLineAfterLogical, bool2TEXT( shouldBreakLineAfterLogical ), iniFilePath );
	// Objective-C Options
	::WritePrivateProfileString( keySectionName, keyAlignMethodColon, bool2TEXT( shouldAlignMethodColon ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyPadMethodPrefix, bool2TEXT( shouldPadMethodPrefix ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyUnPadMethodPrefix, bool2TEXT( shouldUnPadMethodPrefix ), iniFilePath );
	_itot( objCColonPadMode, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyObjCColonPadMode, buffer, iniFilePath );
}
