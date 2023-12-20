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

// #define keySectionName TEXT( "NppAStyle" )
// Tab Options
#define keySameAsNppCurView TEXT( "SameAsNppCurView" )
#define keyUseSpace TEXT( "UseSpace" )
#define keyTabSize TEXT( "TabSize" )

#define keyCurBraceStyle TEXT( "BraceStyle" )
#define keyPointerAlignment TEXT( "PointerAlignment" )
#define keyReferenceAlignment TEXT( "ReferenceAlignment" )
#define keyLineEnd TEXT( "LineEnd" )
// Brace Modify Options
#define keyAttachNamespace TEXT( "AttachNamespace" )
#define keyAttachClass TEXT( "AttachClass" )
#define keyAttachInline TEXT( "AttachInline" )
#define keyAttachExternC TEXT( "AttachExternC" )
#define keyAttachClosingWhile TEXT( "AttachClosingWhile" )
// Function Declaration Options
#define keyBreakReturnType TEXT( "BreakReturnType" )
#define keyBreakReturnTypeDecl TEXT( "BreakReturnTypeDecl" )
// Indentation Options
#define keyClassIndent TEXT( "ClassIndent" )
#define keyModifierIndent TEXT( "ModifierIndent" )
#define keySwitchIndent TEXT( "SwitchIndent" )
#define keyCaseIndent TEXT( "CaseIndent" )
#define keyNamespaceIndent TEXT( "NamespaceIndent" )
#define keyLambdaIndent TEXT( "LambdaIndent" )
#define keyLabelIndent TEXT( "LabelIndent" )
#define keyIndentPreprocBlock TEXT( "IndentPreprocBlock" )
#define keyIndentPreprocConditional TEXT( "IndentPreprocConditional" )
#define keyIndentPreprocDefine TEXT( "IndentPreprocDefine" )
#define keyIndentCol1Comments TEXT( "IndentCol1Comments" )
#define keyIndentAfterParen TEXT( "IndentAfterParen" )
#define keyContinuationIndent TEXT( "ContinuationIndent" )
#define keyMinConditionalOption TEXT( "MinConditionalOption" )
#define keyMaxContinuationIndent TEXT( "MaxContinuationIndent" )
// Padding Options
#define keyPadCommas TEXT( "PadCommas" )
#define keyPadOperators TEXT( "PadOperators" )
#define keyPadParensOutside TEXT( "PadParensOutside" )
#define keyPadFirstParen TEXT( "PadFirstParen" )
#define keyPadParensInside TEXT( "PadParensInside" )
#define keyPadBrackets TEXT( "PadBrackets" )
#define keyPadHeader TEXT( "PadHeader" )
#define keyUnPadParens TEXT( "UnPadParens" )
#define keyDeleteEmptyLines TEXT( "DeleteEmptyLines" )
#define keyEmptyLineFill TEXT( "EmptyLineFill" )
// Formatting Options
#define keyBreakBlocks TEXT( "BreakBlocks" )
#define keyBreakClosingHeaderBlocks TEXT( "BreakClosingHeaderBlocks" )
#define keyBreakElseIfs TEXT( "BreakElseIfs" )
#define keyAddBraces TEXT( "AddBraces" )
#define keyAddOneLineBraces TEXT( "AddOneLineBraces" )
#define keyRemoveBraces TEXT( "RemoveBraces" )
#define keyBreakOneLineBlocks TEXT( "BreakOneLineBlocks" )
#define keyBreakOneLineStatements TEXT( "BreakOneLineStatements" )
#define keyBreakClosingHeaderBraces TEXT( "BreakClosingHeaderBraces" )
#define keyBreakOneLineHeaders TEXT( "BreakOneLineHeaders" )
#define keyConvertTabs TEXT( "ConvertTabs" )
#define keyCloseTemplates TEXT( "CloseTemplates" )
#define keyStripCommentPrefix TEXT( "StripCommentPrefix" )
#define keyMaxCodeLength TEXT( "MaxCodeLength" )
#define keyBreakLineAfterLogical TEXT( "BreakLineAfterLogical" )
// Objective-C Options
#define keyObjcAlignMethodColon TEXT( "ObjCAlignMethodColon" )
#define keyObjcPadMethodPrefix TEXT( "ObjCPadMethodPrefix" )
#define keyObjcUnPadMethodPrefix TEXT( "ObjCUnPadMethodPrefix" )
#define keyObjcPadReturnType TEXT( "ObjCPadReturnType" )
#define keyObjcUnPadReturnType TEXT( "ObjCUnPadReturnType" )
#define keyObjcPadParamType TEXT( "ObjCPadParamType" )
#define keyObjcUnPadParamType TEXT( "ObjCUnPadParamType" )
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

	// 0 C, 1 C++, 2 Java, 3 C#, 4 Objective-C
	languageMode = 0;

	// Brace Modify Options
	shouldAttachNamespace = false;
	shouldAttachClass = false;
	shouldAttachInline = false;
	shouldAttachExternC = false;
	shouldAttachClosingWhile = false;
	// Function Declaration Options
	shouldBreakReturnType = false;
	shouldBreakReturnTypeDecl = false;
	// Indentation Options
	shouldClassIndent = false;
	shouldModifierIndent = false;
	shouldSwitchIndent = false;
	shouldCaseIndent = false;
	shouldNamespaceIndent = false;
	shouldLambdaIndent = false;
	shouldLabelIndent = false;
	shouldIndentPreprocBlock = false;
	shouldIndentPreprocConditional = false;
	shouldIndentPreprocDefine = false;
	shouldIndentCol1Comments = false;
	shouldIndentAfterParen = false;
	continuationIndent = 1;
	minConditionalOption = astyle::MINCOND_TWO;
	maxContinuationIndent = 40;
	// Padding Options
	shouldPadCommas = false;
	shouldPadOperators = false;
	shouldPadParensOutside = false;
	shouldPadFirstParen = false;
	shouldPadParensInside = false;
	shouldPadBrackets = false;
	shouldPadHeader = false;
	shouldUnPadParens = false;
	shouldDeleteEmptyLines = false;
	shouldEmptyLineFill = false;
	// Formatting Options
	shouldBreakBlocks = false;
	shouldBreakClosingHeaderBlocks = false;
	shouldBreakElseIfs = false;
	shouldAddBraces = false;
	shouldAddOneLineBraces = false;
	shouldRemoveBraces = false;
	shouldBreakOneLineBlocks = true;
	shouldBreakOneLineStatements = true;
	shouldBreakClosingHeaderBraces = false;
	shouldBreakOneLineHeaders = false;
	//shouldConvertTabs = false;
	shouldCloseTemplates = false;
	shouldStripCommentPrefix = false;
	maxCodeLength = std::string::npos;
	shouldBreakLineAfterLogical = false;
	// Objective-C Options
	shouldAlignMethodColon = false;
	shouldPadMethodPrefix = false;
	shouldUnPadMethodPrefix = false;
	shouldPadReturnType = false;
	shouldUnPadReturnType = false;
	shouldPadParamType = false;
	shouldUnPadParamType = false;
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

	// Brace Modify Options
	formatter.setAttachNamespace( shouldAttachNamespace );
	formatter.setAttachClass( shouldAttachClass );
	formatter.setAttachInline( shouldAttachInline );
	formatter.setAttachExternC( shouldAttachExternC );
	formatter.setAttachClosingWhile( shouldAttachClosingWhile );
	// Function Declaration Options
	formatter.setBreakReturnType( shouldBreakReturnType );
	formatter.setBreakReturnTypeDecl( shouldBreakReturnTypeDecl );
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
	formatter.setAfterParenIndent( shouldIndentAfterParen );
	formatter.setContinuationIndentation( continuationIndent );
	formatter.setMinConditionalIndentOption( minConditionalOption );
	formatter.setMaxContinuationIndentLength( maxContinuationIndent );
	formatter.setLambdaIndentation( shouldLambdaIndent );
	// Padding Options
	formatter.setCommaPaddingMode( shouldPadCommas );
	formatter.setOperatorPaddingMode( shouldPadOperators );
	formatter.setParensOutsidePaddingMode( shouldPadParensOutside );
	formatter.setParensFirstPaddingMode( shouldPadFirstParen );
	formatter.setParensInsidePaddingMode( shouldPadParensInside );
	formatter.setBracketsOutsidePaddingMode( shouldPadBrackets );
	formatter.setBracketsInsidePaddingMode( shouldPadBrackets );
	formatter.setParensHeaderPaddingMode( shouldPadHeader );
	formatter.setParensUnPaddingMode( shouldUnPadParens );
	formatter.setDeleteEmptyLinesMode( shouldDeleteEmptyLines );
	formatter.setEmptyLineFill( shouldEmptyLineFill );
	// Formatting Options
	formatter.setBreakBlocksMode( shouldBreakBlocks );
	formatter.setBreakClosingHeaderBlocksMode( shouldBreakClosingHeaderBlocks );
	formatter.setBreakElseIfsMode( shouldBreakElseIfs );
	formatter.setAddBracesMode( shouldAddBraces );
	formatter.setAddOneLineBracesMode( shouldAddOneLineBraces );
	formatter.setRemoveBracesMode( shouldRemoveBraces );
	formatter.setBreakOneLineBlocksMode( shouldBreakOneLineBlocks );
	formatter.setBreakOneLineStatementsMode( shouldBreakOneLineStatements );
	formatter.setBreakClosingHeaderBracesMode( shouldBreakClosingHeaderBraces );
	formatter.setBreakOneLineHeadersMode( shouldBreakOneLineHeaders );
	//formatter.setTabSpaceConversionMode( shouldConvertTabs );
	formatter.setCloseTemplatesMode( shouldCloseTemplates );
	formatter.setStripCommentPrefix( shouldStripCommentPrefix );
	formatter.setMaxCodeLength( maxCodeLength ); // <= 200
	formatter.setBreakAfterMode( shouldBreakLineAfterLogical );
	// Objective-C Options
	formatter.setAlignMethodColon( shouldAlignMethodColon );
	formatter.setMethodPrefixPaddingMode( shouldPadMethodPrefix );
	formatter.setMethodPrefixUnPaddingMode( shouldUnPadMethodPrefix );
	formatter.setReturnTypePaddingMode( shouldPadReturnType );
	formatter.setReturnTypeUnPaddingMode( shouldUnPadReturnType );
	formatter.setParamTypePaddingMode( shouldPadParamType );
	formatter.setParamTypeUnPaddingMode( shouldUnPadParamType );
	formatter.setObjCColonPaddingMode( astyle::ObjCColonPad( objCColonPadMode ) );
}

void NppAStyleOption::loadConfigInfo( const TCHAR * keySectionName, const TCHAR strFilePath[] )
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

	formattingStyle = ::GetPrivateProfileInt( keySectionName, keyCurBraceStyle, formattingStyle, iniFilePath );
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

	// Brace Modify Options
	shouldAttachNamespace = 0 != ::GetPrivateProfileInt( keySectionName, keyAttachNamespace, shouldAttachNamespace, iniFilePath );
	shouldAttachClass = 0 != ::GetPrivateProfileInt( keySectionName, keyAttachClass, shouldAttachClass, iniFilePath );
	shouldAttachInline = 0 != ::GetPrivateProfileInt( keySectionName, keyAttachInline, shouldAttachInline, iniFilePath );
	shouldAttachExternC = 0 != ::GetPrivateProfileInt( keySectionName, keyAttachExternC, shouldAttachExternC, iniFilePath );
	shouldAttachClosingWhile = 0 != ::GetPrivateProfileInt( keySectionName, keyAttachClosingWhile, shouldAttachClosingWhile, iniFilePath );
	// Function Declaration Options
	shouldBreakReturnType = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakReturnType, shouldBreakReturnType, iniFilePath );
	shouldBreakReturnTypeDecl = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakReturnTypeDecl, shouldBreakReturnTypeDecl, iniFilePath );
	// Indentation Options
	shouldClassIndent = 0 != ::GetPrivateProfileInt( keySectionName, keyClassIndent, shouldClassIndent, iniFilePath );
	shouldModifierIndent = 0 != ::GetPrivateProfileInt( keySectionName, keyModifierIndent, shouldModifierIndent, iniFilePath );
	shouldSwitchIndent = 0 != ::GetPrivateProfileInt( keySectionName, keySwitchIndent, shouldSwitchIndent, iniFilePath );
	shouldCaseIndent = 0 != ::GetPrivateProfileInt( keySectionName, keyCaseIndent, shouldCaseIndent, iniFilePath );
	shouldNamespaceIndent = 0 != ::GetPrivateProfileInt( keySectionName, keyNamespaceIndent, shouldNamespaceIndent, iniFilePath );
	shouldLambdaIndent = 0 != ::GetPrivateProfileInt( keySectionName, keyLambdaIndent, shouldLambdaIndent, iniFilePath );
	shouldLabelIndent = 0 != ::GetPrivateProfileInt( keySectionName, keyLabelIndent, shouldLabelIndent, iniFilePath );
	shouldIndentPreprocBlock = 0 != ::GetPrivateProfileInt( keySectionName, keyIndentPreprocBlock, shouldIndentPreprocBlock, iniFilePath );
	shouldIndentPreprocConditional = 0 != ::GetPrivateProfileInt( keySectionName, keyIndentPreprocConditional, shouldIndentPreprocConditional, iniFilePath );
	shouldIndentPreprocDefine = 0 != ::GetPrivateProfileInt( keySectionName, keyIndentPreprocDefine, shouldIndentPreprocDefine, iniFilePath );
	shouldIndentCol1Comments = 0 != ::GetPrivateProfileInt( keySectionName, keyIndentCol1Comments, shouldIndentCol1Comments, iniFilePath );
	shouldIndentAfterParen = 0 != ::GetPrivateProfileInt( keySectionName, keyIndentAfterParen, shouldIndentAfterParen, iniFilePath );
	continuationIndent = ::GetPrivateProfileInt( keySectionName, keyContinuationIndent, continuationIndent, iniFilePath );
	continuationIndent  = ( continuationIndent < 0 || continuationIndent > 4 ) ? 1 : continuationIndent;
	minConditionalOption = ::GetPrivateProfileInt( keySectionName, keyMinConditionalOption, minConditionalOption, iniFilePath );
	minConditionalOption  = ( minConditionalOption < astyle::MINCOND_ZERO || minConditionalOption > astyle::MINCOND_ONEHALF ) ? astyle::MINCOND_TWO : minConditionalOption;
	maxContinuationIndent = ::GetPrivateProfileInt( keySectionName, keyMaxContinuationIndent, maxContinuationIndent, iniFilePath );
	maxContinuationIndent  = ( maxContinuationIndent < 40 || maxContinuationIndent > 120 ) ? 40 : maxContinuationIndent;
	// Padding Options
	shouldPadCommas = 0 != ::GetPrivateProfileInt( keySectionName, keyPadCommas, shouldPadCommas, iniFilePath );
	shouldPadOperators = 0 != ::GetPrivateProfileInt( keySectionName, keyPadOperators, shouldPadOperators, iniFilePath );
	shouldPadParensOutside = 0 != ::GetPrivateProfileInt( keySectionName, keyPadParensOutside, shouldPadParensOutside, iniFilePath );
	shouldPadFirstParen = 0 != ::GetPrivateProfileInt( keySectionName, keyPadFirstParen, shouldPadFirstParen, iniFilePath );
	shouldPadParensInside = 0 != ::GetPrivateProfileInt( keySectionName, keyPadParensInside, shouldPadParensInside, iniFilePath );
	shouldPadBrackets = 0 != ::GetPrivateProfileInt( keySectionName, keyPadBrackets, shouldPadBrackets, iniFilePath );
	shouldPadHeader = 0 != ::GetPrivateProfileInt( keySectionName, keyPadHeader, shouldPadHeader, iniFilePath );
	shouldUnPadParens = 0 != ::GetPrivateProfileInt( keySectionName, keyUnPadParens, shouldUnPadParens, iniFilePath );
	shouldDeleteEmptyLines = 0 != ::GetPrivateProfileInt( keySectionName, keyDeleteEmptyLines, shouldDeleteEmptyLines, iniFilePath );
	shouldEmptyLineFill = 0 != ::GetPrivateProfileInt( keySectionName, keyEmptyLineFill, shouldEmptyLineFill, iniFilePath );
	// Formatting Options
	shouldBreakBlocks = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakBlocks, shouldBreakBlocks, iniFilePath );
	shouldBreakClosingHeaderBlocks = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakClosingHeaderBlocks, shouldBreakClosingHeaderBlocks, iniFilePath );
	shouldBreakElseIfs = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakElseIfs, shouldBreakElseIfs, iniFilePath );
	shouldAddBraces = 0 != ::GetPrivateProfileInt( keySectionName, keyAddBraces, shouldAddBraces, iniFilePath );
	shouldAddOneLineBraces = 0 != ::GetPrivateProfileInt( keySectionName, keyAddOneLineBraces, shouldAddOneLineBraces, iniFilePath );
	shouldRemoveBraces = 0 != ::GetPrivateProfileInt( keySectionName, keyRemoveBraces, shouldRemoveBraces, iniFilePath );
	shouldBreakOneLineBlocks = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakOneLineBlocks, shouldBreakOneLineBlocks, iniFilePath );
	shouldBreakOneLineStatements = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakOneLineStatements, shouldBreakOneLineStatements, iniFilePath );
	shouldBreakClosingHeaderBraces = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakClosingHeaderBraces, shouldBreakClosingHeaderBraces, iniFilePath );
	shouldBreakOneLineHeaders = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakOneLineHeaders, shouldBreakOneLineHeaders, iniFilePath );
	//shouldConvertTabs = 0 != ::GetPrivateProfileInt( keySectionName, keyConvertTabs, shouldConvertTabs, iniFilePath );
	shouldCloseTemplates = 0 != ::GetPrivateProfileInt( keySectionName, keyCloseTemplates, shouldCloseTemplates, iniFilePath );
	shouldStripCommentPrefix = 0 != ::GetPrivateProfileInt( keySectionName, keyStripCommentPrefix, shouldStripCommentPrefix, iniFilePath );
	maxCodeLength = ::GetPrivateProfileInt( keySectionName, keyMaxCodeLength, maxCodeLength, iniFilePath );
	maxCodeLength = ( maxCodeLength < 50 || maxCodeLength > 200 ) ? std::string::npos : maxCodeLength;
	shouldBreakLineAfterLogical = 0 != ::GetPrivateProfileInt( keySectionName, keyBreakLineAfterLogical, shouldBreakLineAfterLogical, iniFilePath );
	// Objective-C Options
	shouldAlignMethodColon = 0 != ::GetPrivateProfileInt( keySectionName, keyObjcAlignMethodColon, shouldAlignMethodColon, iniFilePath );
	shouldPadMethodPrefix = 0 != ::GetPrivateProfileInt( keySectionName, keyObjcPadMethodPrefix, shouldPadMethodPrefix, iniFilePath );
	shouldUnPadMethodPrefix = 0 != ::GetPrivateProfileInt( keySectionName, keyObjcUnPadMethodPrefix, shouldUnPadMethodPrefix, iniFilePath );
	shouldPadReturnType = 0 != ::GetPrivateProfileInt( keySectionName, keyObjcPadReturnType, shouldPadReturnType, iniFilePath );
	shouldUnPadReturnType = 0 != ::GetPrivateProfileInt( keySectionName, keyObjcUnPadReturnType, shouldUnPadReturnType, iniFilePath );
	shouldPadParamType = 0 != ::GetPrivateProfileInt( keySectionName, keyObjcPadParamType, shouldPadParamType, iniFilePath );
	shouldUnPadParamType = 0 != ::GetPrivateProfileInt( keySectionName, keyObjcUnPadParamType, shouldUnPadParamType, iniFilePath );
	objCColonPadMode = ::GetPrivateProfileInt( keySectionName, keyObjCColonPadMode, objCColonPadMode, iniFilePath );
	objCColonPadMode = ( objCColonPadMode < astyle::COLON_PAD_NO_CHANGE || objCColonPadMode > astyle::COLON_PAD_BEFORE ) ? astyle::COLON_PAD_NO_CHANGE : objCColonPadMode;
}

void NppAStyleOption::saveConfigInfo( const TCHAR * keySectionName, const TCHAR strFilePath[] ) const
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
	::WritePrivateProfileString( keySectionName, keyCurBraceStyle, buffer, iniFilePath );

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

	// Brace Modify Options
	::WritePrivateProfileString( keySectionName, keyAttachNamespace, bool2TEXT( shouldAttachNamespace ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyAttachClass, bool2TEXT( shouldAttachClass ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyAttachInline, bool2TEXT( shouldAttachInline ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyAttachExternC, bool2TEXT( shouldAttachExternC ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyAttachClosingWhile , bool2TEXT( shouldAttachClosingWhile ), iniFilePath );
	// Function Declaration Options
	::WritePrivateProfileString( keySectionName, keyBreakReturnType, bool2TEXT( shouldBreakReturnType ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakReturnTypeDecl, bool2TEXT( shouldBreakReturnTypeDecl ), iniFilePath );
	// Indentation Options
	::WritePrivateProfileString( keySectionName, keyClassIndent, bool2TEXT( shouldClassIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyModifierIndent, bool2TEXT( shouldModifierIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keySwitchIndent, bool2TEXT( shouldSwitchIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyCaseIndent, bool2TEXT( shouldCaseIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyNamespaceIndent, bool2TEXT( shouldNamespaceIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyLambdaIndent, bool2TEXT( shouldLambdaIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyLabelIndent, bool2TEXT( shouldLabelIndent ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyIndentPreprocBlock, bool2TEXT( shouldIndentPreprocBlock ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyIndentPreprocConditional, bool2TEXT( shouldIndentPreprocConditional ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyIndentPreprocDefine, bool2TEXT( shouldIndentPreprocDefine ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyIndentCol1Comments, bool2TEXT( shouldIndentCol1Comments ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyIndentAfterParen, bool2TEXT( shouldIndentAfterParen ), iniFilePath );
	_itot( continuationIndent, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyContinuationIndent, buffer, iniFilePath );
	_itot( minConditionalOption, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyMinConditionalOption, buffer, iniFilePath );
	_itot( maxContinuationIndent, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyMaxContinuationIndent, buffer, iniFilePath );
	// Padding Options
	::WritePrivateProfileString( keySectionName, keyPadCommas, bool2TEXT( shouldPadCommas ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyPadOperators, bool2TEXT( shouldPadOperators ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyPadParensOutside, bool2TEXT( shouldPadParensOutside ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyPadFirstParen, bool2TEXT( shouldPadFirstParen ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyPadParensInside, bool2TEXT( shouldPadParensInside ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyPadBrackets, bool2TEXT( shouldPadBrackets ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyPadHeader, bool2TEXT( shouldPadHeader ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyUnPadParens, bool2TEXT( shouldUnPadParens ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyDeleteEmptyLines, bool2TEXT( shouldDeleteEmptyLines ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyEmptyLineFill, bool2TEXT( shouldEmptyLineFill ), iniFilePath );
	// Formatting Options
	::WritePrivateProfileString( keySectionName, keyBreakBlocks, bool2TEXT( shouldBreakBlocks ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakClosingHeaderBlocks, bool2TEXT( shouldBreakClosingHeaderBlocks ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakElseIfs, bool2TEXT( shouldBreakElseIfs ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyAddBraces, bool2TEXT( shouldAddBraces ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyAddOneLineBraces, bool2TEXT( shouldAddOneLineBraces ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyRemoveBraces, bool2TEXT( shouldRemoveBraces ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakOneLineBlocks, bool2TEXT( shouldBreakOneLineBlocks ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakOneLineStatements, bool2TEXT( shouldBreakOneLineStatements ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakClosingHeaderBraces, bool2TEXT( shouldBreakClosingHeaderBraces ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakOneLineHeaders, bool2TEXT( shouldBreakOneLineHeaders ), iniFilePath );
	//::WritePrivateProfileString( keySectionName, keyConvertTabs, bool2TEXT( shouldConvertTabs ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyCloseTemplates, bool2TEXT( shouldCloseTemplates ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyStripCommentPrefix, bool2TEXT( shouldStripCommentPrefix ), iniFilePath );
	_itot( maxCodeLength, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyMaxCodeLength, buffer, iniFilePath );
	::WritePrivateProfileString( keySectionName, keyBreakLineAfterLogical, bool2TEXT( shouldBreakLineAfterLogical ), iniFilePath );
	// Objective-C Options
	::WritePrivateProfileString( keySectionName, keyObjcAlignMethodColon, bool2TEXT( shouldAlignMethodColon ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyObjcPadMethodPrefix, bool2TEXT( shouldPadMethodPrefix ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyObjcUnPadMethodPrefix, bool2TEXT( shouldUnPadMethodPrefix ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyObjcPadReturnType, bool2TEXT( shouldPadReturnType ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyObjcUnPadReturnType, bool2TEXT( shouldUnPadReturnType ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyObjcPadParamType, bool2TEXT( shouldPadParamType ), iniFilePath );
	::WritePrivateProfileString( keySectionName, keyObjcUnPadParamType, bool2TEXT( shouldUnPadParamType ), iniFilePath );
	_itot( objCColonPadMode, buffer, 10 );
	::WritePrivateProfileString( keySectionName, keyObjCColonPadMode, buffer, iniFilePath );
}



#define keySectionNamePrefix TEXT( "NppAStyle" )

// 0 C, 1 C++, 2 Java, 3 C#, 4 Objective-C
const TCHAR * NppAStyleOptionSet::languageNames[NppAStyleOptionSet::languageCount] = {
	TEXT( "C" ),
	TEXT( "C++" ),
	TEXT( "Java" ),
	TEXT( "C#" ),
	TEXT( "Objective-C" ),
	TEXT( "JavaScript" )
};

// 0 C, 1 C++, 2 Java, 3 C#, 4 Objective-C
const TCHAR * NppAStyleOptionSet::languageSectionNames[NppAStyleOptionSet::languageCount] = {
	keySectionNamePrefix TEXT( " " ) TEXT( "C" ),
	keySectionNamePrefix TEXT( " " ) TEXT( "C++" ),
	keySectionNamePrefix TEXT( " " ) TEXT( "Java" ),
	keySectionNamePrefix TEXT( " " ) TEXT( "C#" ),
	keySectionNamePrefix TEXT( " " ) TEXT( "Objective-C" ),
	keySectionNamePrefix TEXT( " " ) TEXT( "JavaScript" )
};

void NppAStyleOptionSet::loadConfigInfo( const TCHAR strFilePath[] )
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

	for (int i = 0; i < languageCount; ++i )
	{
		languageAStyleOption[i].loadConfigInfo( languageSectionNames[i], iniFilePath);
	}
}

void NppAStyleOptionSet::saveConfigInfo( const TCHAR strFilePath[] ) const
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

	for (int i = 0; i < languageCount; ++i )
	{
		languageAStyleOption[i].saveConfigInfo( languageSectionNames[i], iniFilePath);
	}
}

