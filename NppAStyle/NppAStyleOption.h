/***************************************************************
 * Name:        NppAStyleOption.h
 * Purpose:     declaration of NppAStyleOption struct
 * Author:      YWX (wxFortranIndent@163.com)
 * Created:     2015-7-26
 * Copyright:   (c) YWX <wxFortranIndent@163.com>
 * Licence:     GNU General Public License, version 3
 **************************************************************/

#ifndef _NPP_ASTYLE_OPTION_H_
#define _NPP_ASTYLE_OPTION_H_

namespace astyle
{
class ASFormatter;
}

struct NppAStyleOption
{
	NppAStyleOption()
	{
		reset();
	}
	~NppAStyleOption() {}

	void reset();
	void setFormatterOption( astyle::ASFormatter &formatter ) const;
	void loadConfigInfo( const TCHAR strFilePath[] = NULL );
	void saveConfigInfo( const TCHAR strFilePath[] = NULL ) const;

	// Tab Options
	bool isSameAsNppCurView;
	bool isUseSpace;
	int iTabSize;

	int formattingStyle;
	int pointerAlignment; // astyle::PointerAlign
	int referenceAlignment; // astyle::ReferenceAlign
	//int lineEnd; // astyle::LineEndFormat
	int languageMode;

	// Bracket Modify Options
	bool shouldAttachNamespace;
	bool shouldAttachClass;
	bool shouldAttachInline;
	bool shouldAttachExternC;
	// Indentation Options
	bool shouldClassIndent;
	bool shouldModifierIndent;
	bool shouldSwitchIndent;
	bool shouldCaseIndent;
	bool shouldNamespaceIndent;
	bool shouldLabelIndent;
	bool shouldIndentPreprocBlock;
	bool shouldIndentPreprocConditional;
	bool shouldIndentPreprocDefine;
	bool shouldIndentCol1Comments;
	int  continuationIndent;
	int  minConditionalOption;
	int  maxInStatementIndent;
	// Padding Options
	bool shouldPadCommas;
	bool shouldPadOperators;
	bool shouldPadParensOutside;
	bool shouldPadFirstParen;
	bool shouldPadParensInside;
	bool shouldPadHeader;
	bool shouldUnPadParens;
	bool shouldDeleteEmptyLines;
	bool shouldEmptyLineFill;
	// Formatting Options
	bool shouldBreakBlocks;
	bool shouldBreakClosingHeaderBlocks;
	bool shouldBreakElseIfs;
	bool shouldAddBrackets;
	bool shouldAddOneLineBrackets;
	bool shouldRemoveBrackets;
	bool shouldBreakOneLineBlocks;
	bool shouldBreakOneLineStatements;
	bool shouldBreakClosingHeaderBrackets;
	bool shouldBreakOneLineHeaders;
	//bool shouldConvertTabs;
	bool shouldCloseTemplates;
	bool shouldStripCommentPrefix;
	size_t maxCodeLength;
	bool shouldBreakLineAfterLogical;
	// Objective-C Options
	bool shouldAlignMethodColon;
	bool shouldPadMethodPrefix;
	bool shouldUnPadMethodPrefix;
	bool shouldPadReturnType;
	bool shouldUnPadReturnType;
	bool shouldPadParamType;
	bool shouldUnPadParamType;
	int objCColonPadMode; // astyle::ObjCColonPad
};

#endif // _NPP_ASTYLE_OPTION_H_
