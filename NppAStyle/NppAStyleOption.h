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
	void loadConfigInfo( const TCHAR * keySectionName = NULL, const TCHAR strFilePath[] = NULL );
	void saveConfigInfo( const TCHAR * keySectionName = NULL, const TCHAR strFilePath[] = NULL ) const;

	// Tab Options
	bool isSameAsNppCurView;
	bool isUseSpace;
	int iTabSize;

	int formattingStyle;
	int pointerAlignment; // astyle::PointerAlign
	int referenceAlignment; // astyle::ReferenceAlign
	//int lineEnd; // astyle::LineEndFormat

	// 0 C, 1 C++, 2 Java, 3 C#, 4 Objective-C, 5 JavaScript
	int languageMode;

	// Brace Modify Options
	bool shouldAttachNamespace;
	bool shouldAttachClass;
	bool shouldAttachInline;
	bool shouldAttachExternC;
	bool shouldAttachClosingWhile;
	// Function Declaration Options
	bool shouldBreakReturnType;
	bool shouldBreakReturnTypeDecl;
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
	bool shouldIndentAfterParen;
	bool shouldLambdaIndent;
	int  continuationIndent;
	int  minConditionalOption;
	int  maxContinuationIndent;
	// Padding Options
	bool shouldPadCommas;
	bool shouldPadOperators;
	bool shouldPadParensOutside;
	bool shouldPadFirstParen;
	bool shouldPadParensInside;
	bool shouldPadBrackets;
	bool shouldPadHeader;
	bool shouldUnPadParens;
	bool shouldDeleteEmptyLines;
	bool shouldEmptyLineFill;
	// Formatting Options
	bool shouldBreakBlocks;
	bool shouldBreakClosingHeaderBlocks;
	bool shouldBreakElseIfs;
	bool shouldAddBraces;
	bool shouldAddOneLineBraces;
	bool shouldRemoveBraces;
	bool shouldBreakOneLineBlocks;
	bool shouldBreakOneLineStatements;
	bool shouldBreakClosingHeaderBraces;
	bool shouldBreakOneLineHeaders;
	// bool shouldConvertTabs;
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

struct NppAStyleOptionSet
{
	NppAStyleOptionSet()
	{
		for (int i = 0; i < languageCount; ++i )
		{
			languageAStyleOption[i].languageMode = i;
		}
	}

	~NppAStyleOptionSet() {}

	void loadConfigInfo( const TCHAR strFilePath[] = NULL );
	void saveConfigInfo( const TCHAR strFilePath[] = NULL ) const;

	// 0 C, 1 C++, 2 Java, 3 C#, 4 Objective-C, 5 JavaScript
	static const int languageCount = 6;

	static const TCHAR * languageNames[languageCount];

	static const TCHAR * languageSectionNames[languageCount];

	NppAStyleOption languageAStyleOption[languageCount];
};

#endif // _NPP_ASTYLE_OPTION_H_
