// astyle.h
// Copyright (c) 2023 The Artistic Style Authors.
// This code is licensed under the MIT License.
// License.md describes the conditions under which this software may be distributed.

#ifndef ASTYLE_H
#define ASTYLE_H

//-----------------------------------------------------------------------------
// headers
//-----------------------------------------------------------------------------

#ifdef __VMS
	#define __USE_STD_IOSTREAM 1
	#include <assert>
#else
	#include <cassert>
#endif

#include <cctype>
#include <iostream>		// for cout
#include <memory>
#include <string>
#include <vector>

#ifdef __GNUC__
	#include <cstring>              // need both string and cstring for GCC
#endif

//-----------------------------------------------------------------------------
// declarations
//-----------------------------------------------------------------------------

#ifdef _MSC_VER
	#pragma warning(disable: 4267)  // conversion from size_t to int
#endif

#ifdef __BORLANDC__
	#pragma warn -8004	            // variable is assigned a value that is never used
#endif

#ifdef __GNUC__
	#pragma GCC diagnostic ignored "-Wconversion"
#endif

#ifdef __INTEL_COMPILER
	// #pragma warning disable 383  // value copied to temporary, reference to temporary used
	// #pragma warning disable 981  // operands are evaluated in unspecified order
#endif

#ifdef __clang__
	#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif

#define ASTYLE_VERSION "3.4.10"

//-----------------------------------------------------------------------------
// astyle namespace
//-----------------------------------------------------------------------------

namespace astyle {

//----------------------------------------------------------------------------
// definitions
//----------------------------------------------------------------------------

enum FileType { C_TYPE = 0, JAVA_TYPE = 1, SHARP_TYPE = 2, JS_TYPE = 3, OBJC_TYPE = 4, INVALID_TYPE = -1 };

/* The enums below are not recognized by 'vectors' in Microsoft Visual C++
   V5 when they are part of a namespace!!!  Use Visual C++ V6 or higher.
*/
enum FormatStyle
{
	STYLE_NONE,
	STYLE_ALLMAN,
	STYLE_JAVA,
	STYLE_KR,
	STYLE_STROUSTRUP,
	STYLE_WHITESMITH,
	STYLE_VTK,
	STYLE_RATLIFF,
	STYLE_GNU,
	STYLE_LINUX,
	STYLE_HORSTMANN,
	STYLE_1TBS,
	STYLE_GOOGLE,
	STYLE_MOZILLA,
	STYLE_WEBKIT,
	STYLE_PICO,
	STYLE_LISP
};

enum BraceMode
{
	NONE_MODE,
	ATTACH_MODE,
	BREAK_MODE,
	LINUX_MODE,
	RUN_IN_MODE		// broken braces
};

// maximum single value for size_t is 32,768 (total value of 65,535)
enum BraceType : size_t
{
	NULL_TYPE        = 0,
	NAMESPACE_TYPE   = 1,		// also a DEFINITION_TYPE
	CLASS_TYPE       = 2,		// also a DEFINITION_TYPE
	STRUCT_TYPE      = 4,		// also a DEFINITION_TYPE
	INTERFACE_TYPE   = 8,		// also a DEFINITION_TYPE
	DEFINITION_TYPE  = 16,
	COMMAND_TYPE     = 32,
	ARRAY_NIS_TYPE   = 64,		// also an ARRAY_TYPE
	ENUM_TYPE        = 128,		// also an ARRAY_TYPE
	INIT_TYPE        = 256,		// also an ARRAY_TYPE
	ARRAY_TYPE       = 512,
	EXTERN_TYPE      = 1024,	// extern "C", not a command type extern
	EMPTY_BLOCK_TYPE = 2048,	// also a SINGLE_LINE_TYPE
	BREAK_BLOCK_TYPE = 4096,	// also a SINGLE_LINE_TYPE
	SINGLE_LINE_TYPE = 8192
};

enum MinConditional
{
	MINCOND_ZERO,
	MINCOND_ONE,
	MINCOND_TWO,
	MINCOND_ONEHALF,
	MINCOND_END
};

enum ObjCColonPad
{
	COLON_PAD_NO_CHANGE,
	COLON_PAD_NONE,
	COLON_PAD_ALL,
	COLON_PAD_AFTER,
	COLON_PAD_BEFORE
};

enum PointerAlign
{
	PTR_ALIGN_NONE,
	PTR_ALIGN_TYPE,
	PTR_ALIGN_MIDDLE,
	PTR_ALIGN_NAME
};

enum ReferenceAlign
{
	REF_ALIGN_NONE   = PTR_ALIGN_NONE,
	REF_ALIGN_TYPE   = PTR_ALIGN_TYPE,
	REF_ALIGN_MIDDLE = PTR_ALIGN_MIDDLE,
	REF_ALIGN_NAME   = PTR_ALIGN_NAME,
	REF_SAME_AS_PTR
};

enum FileEncoding
{
	ENCODING_8BIT,  // includes UTF-8 without BOM
	UTF_8BOM,       // UTF-8 with BOM
	UTF_16BE,
	UTF_16LE,       // Windows default
	UTF_32BE,
	UTF_32LE
};

enum LineEndFormat
{
	LINEEND_DEFAULT,	// Use line break that matches most of the file
	LINEEND_WINDOWS,
	LINEEND_LINUX,
	LINEEND_MACOLD,
	LINEEND_CRLF = LINEEND_WINDOWS,
	LINEEND_LF   = LINEEND_LINUX,
	LINEEND_CR   = LINEEND_MACOLD
};

//-----------------------------------------------------------------------------
// Class ASSourceIterator
// A pure virtual class is used by ASFormatter and ASBeautifier instead of
// ASStreamIterator. This allows programs using AStyle as a plug-in to define
// their own ASStreamIterator. The ASStreamIterator class must inherit
// this class.
//-----------------------------------------------------------------------------

class ASSourceIterator
{
public:
	ASSourceIterator() = default;
	virtual ~ASSourceIterator() = default;
	virtual std::streamoff getPeekStart() const = 0;
	virtual int getStreamLength() const = 0;
	virtual bool hasMoreLines() const = 0;
	virtual std::string nextLine(bool emptyLineWasDeleted) = 0;
	virtual std::string peekNextLine() = 0;
	virtual void peekReset() = 0;
	virtual std::streamoff tellg() = 0;
};

//-----------------------------------------------------------------------------
// Class ASPeekStream
// A small class using RAII to peek ahead in the ASSourceIterator stream
// and to reset the ASSourceIterator pointer in the destructor.
// It enables a return from anywhere in the method.
//-----------------------------------------------------------------------------

class ASPeekStream
{
private:
	ASSourceIterator* sourceIterator;
	bool needReset;		// reset sourceIterator to the original position

public:
	explicit ASPeekStream(ASSourceIterator* sourceIterator_)
	{ sourceIterator = sourceIterator_; needReset = false; }

	~ASPeekStream()
	{ if (needReset) sourceIterator->peekReset(); }

	bool hasMoreLines() const
	{ return sourceIterator->hasMoreLines(); }

	std::string peekNextLine()
	{ needReset = true; return sourceIterator->peekNextLine(); }
};


//-----------------------------------------------------------------------------
// Class ASResource
//-----------------------------------------------------------------------------

class ASResource
{
public:
	void buildAssignmentOperators(std::vector<const std::string*>* assignmentOperators);
	void buildCastOperators(std::vector<const std::string*>* castOperators);
	void buildHeaders(std::vector<const std::string*>* headers, int fileType, bool beautifier = false);
	void buildIndentableMacros(std::vector<const std::pair<const std::string, const std::string>* >* indentableMacros);
	void buildIndentableHeaders(std::vector<const std::string*>* indentableHeaders);
	void buildNonAssignmentOperators(std::vector<const std::string*>* nonAssignmentOperators);
	void buildNonParenHeaders(std::vector<const std::string*>* nonParenHeaders, int fileType, bool beautifier = false);
	void buildOperators(std::vector<const std::string*>* operators, int fileType);
	void buildPreBlockStatements(std::vector<const std::string*>* preBlockStatements, int fileType);
	void buildPreCommandHeaders(std::vector<const std::string*>* preCommandHeaders, int fileType);
	void buildPreDefinitionHeaders(std::vector<const std::string*>* preDefinitionHeaders, int fileType);

public:
	static const std::string AS_IF, AS_ELSE;
	static const std::string AS_DO, AS_WHILE;
	static const std::string AS_FOR;
	static const std::string AS_SWITCH, AS_CASE, AS_DEFAULT;
	static const std::string AS_TRY, AS_CATCH, AS_THROW, AS_THROWS, AS_FINALLY, AS_USING;
	static const std::string _AS_TRY, _AS_FINALLY, _AS_EXCEPT;
	static const std::string AS_PUBLIC, AS_PROTECTED, AS_PRIVATE;
	static const std::string AS_CLASS, AS_STRUCT, AS_TYPEDEF_STRUCT, AS_UNION, AS_INTERFACE, AS_NAMESPACE;
	static const std::string AS_MODULE;
	static const std::string AS_END;
	static const std::string AS_SELECTOR;
	static const std::string AS_EXTERN, AS_ENUM;
	static const std::string AS_FINAL, AS_OVERRIDE;
	static const std::string AS_STATIC, AS_CONST, AS_SEALED, AS_VOLATILE, AS_NEW, AS_DELETE;
	static const std::string AS_NOEXCEPT, AS_INTERRUPT, AS_AUTORELEASEPOOL;
	static const std::string AS_WHERE, AS_LET, AS_SYNCHRONIZED;
	static const std::string AS_OPERATOR, AS_TEMPLATE;
	static const std::string AS_OPEN_PAREN, AS_CLOSE_PAREN;
	static const std::string AS_OPEN_BRACE, AS_CLOSE_BRACE;
	static const std::string AS_OPEN_LINE_COMMENT, AS_OPEN_COMMENT, AS_CLOSE_COMMENT;
	static const std::string AS_BAR_DEFINE, AS_BAR_INCLUDE, AS_BAR_IF, AS_BAR_EL, AS_BAR_ENDIF;
	static const std::string AS_AUTO, AS_RETURN;
	static const std::string AS_CIN, AS_COUT, AS_CERR, AS_MAPPING;
	static const std::string AS_ASSIGN, AS_PLUS_ASSIGN, AS_MINUS_ASSIGN, AS_MULT_ASSIGN;
	static const std::string AS_DIV_ASSIGN, AS_MOD_ASSIGN, AS_XOR_ASSIGN, AS_OR_ASSIGN, AS_AND_ASSIGN;
	static const std::string AS_GR_GR_ASSIGN, AS_LS_LS_ASSIGN, AS_GR_GR_GR_ASSIGN, AS_LS_LS_LS_ASSIGN;
	static const std::string AS_GCC_MIN_ASSIGN, AS_GCC_MAX_ASSIGN, AS_SPACESHIP, AS_EQUAL_JS, AS_COALESCE_CS;
	static const std::string AS_EQUAL, AS_PLUS_PLUS, AS_MINUS_MINUS, AS_NOT_EQUAL, AS_GR_EQUAL;
	static const std::string AS_LS_EQUAL, AS_LS_LS_LS, AS_LS_LS, AS_GR_GR_GR, AS_GR_GR;
	static const std::string AS_QUESTION_QUESTION, AS_LAMBDA;
	static const std::string AS_ARROW, AS_AND, AS_OR;
	static const std::string AS_SCOPE_RESOLUTION;
	static const std::string AS_PLUS, AS_MINUS, AS_MULT, AS_DIV, AS_MOD, AS_GR, AS_LS;
	static const std::string AS_NOT, AS_BIT_XOR, AS_BIT_OR, AS_BIT_AND, AS_BIT_NOT;
	static const std::string AS_QUESTION, AS_COLON, AS_SEMICOLON, AS_COMMA, AS_DOT;
	static const std::string AS_ASM, AS__ASM__, AS_MS_ASM, AS_MS__ASM;
	static const std::string AS_QFOREACH, AS_QFOREVER, AS_FOREVER;
	static const std::string AS_FOREACH, AS_LOCK, AS_UNSAFE, AS_FIXED;
	static const std::string AS_GET, AS_SET, AS_ADD, AS_REMOVE;
	static const std::string AS_DELEGATE, AS_UNCHECKED;
	static const std::string AS_CONST_CAST, AS_DYNAMIC_CAST, AS_REINTERPRET_CAST, AS_STATIC_CAST;
	static const std::string AS_NS_DURING, AS_NS_HANDLER;
};  // Class ASResource

//-----------------------------------------------------------------------------
// Class ASBase
// Functions definitions are at the end of ASResource.cpp.
//-----------------------------------------------------------------------------

class ASBase : protected ASResource
{
private:
	// all variables should be set by the "init" function
	int baseFileType = C_TYPE;      // a value from enum FileType

protected:
	ASBase() = default;

protected:  // inline functions
	void init(int fileTypeArg) { baseFileType = fileTypeArg; }
	bool isCStyle() const { return baseFileType == C_TYPE || baseFileType == OBJC_TYPE; }
	bool isJavaStyle() const { return baseFileType == JAVA_TYPE; }
	bool isSharpStyle() const { return baseFileType == SHARP_TYPE; }
	bool isJSStyle() const { return baseFileType == JS_TYPE; }
	bool isObjCStyle() const { return baseFileType == OBJC_TYPE; }

	bool isWhiteSpace(char ch) const { return (ch == ' ' || ch == '\t'); }

protected:  // functions definitions are at the end of ASResource.cpp
	const std::string* findHeader(const std::string& line, int i,
	                              const std::vector<const std::string*>* possibleHeaders) const;
	bool findKeyword(const std::string& line, int i, const std::string& keyword) const;
	const std::string* findOperator(const std::string& line, int i,
	                                const std::vector<const std::string*>* possibleOperators) const;
	std::string getCurrentWord(const std::string& line, size_t index) const;
	bool isDigit(char ch) const;
	bool isLegalNameChar(char ch) const;
	bool isCharPotentialHeader(const std::string& line, size_t i) const;
	bool isCharPotentialOperator(char ch) const;
	bool isDigitSeparator(const std::string& line, int i) const;
	char peekNextChar(const std::string& line, int i) const;

};  // Class ASBase

//-----------------------------------------------------------------------------
// Class ASBeautifier
//-----------------------------------------------------------------------------

class ASBeautifier : protected ASBase
{
public:
	ASBeautifier();
	virtual ~ASBeautifier();
	ASBeautifier(const ASBeautifier& other);
	ASBeautifier& operator=(ASBeautifier const&) = delete;
	ASBeautifier(ASBeautifier&&)                 = delete;
	ASBeautifier& operator=(ASBeautifier&&)      = delete;
	virtual void init(ASSourceIterator* iter);

	virtual std::string beautify(const std::string& originalLine);
	void setCaseIndent(bool state);
	void setClassIndent(bool state);
	void setContinuationIndentation(int indent = 1);
	void setCStyle();
	void setDefaultTabLength();
	void setEmptyLineFill(bool state);
	void setForceTabXIndentation(int length);
	void setAfterParenIndent(bool state);
	void setJavaStyle();
	void setJSStyle();
	void setObjCStyle();
	void setSharpStyle();

	void setLabelIndent(bool state);
	void setMaxContinuationIndentLength(int max);
	void setMaxInStatementIndentLength(int max);
	void setMinConditionalIndentOption(int min);
	void setMinConditionalIndentLength();
	void setModeManuallySet(bool state);
	void setModifierIndent(bool state);
	void setNamespaceIndent(bool state);
	void setAlignMethodColon(bool state);
	void setSpaceIndentation(int length = 4);
	void setSwitchIndent(bool state);
	void setTabIndentation(int length = 4, bool forceTabs = false);
	void setPreprocDefineIndent(bool state);
	void setPreprocConditionalIndent(bool state);
	void setSqueezeWhitespace(bool state);
	void setLambdaIndentation(bool state);
	int  getBeautifierFileType() const;
	int  getFileType() const;
	int  getIndentLength() const;
	int  getTabLength() const;
	std::string getIndentString() const;
	std::string getNextWord(const std::string& line, size_t currPos) const;
	bool getAlignMethodColon() const;
	bool getBraceIndent() const;
	bool getBlockIndent() const;
	bool getCaseIndent() const;
	bool getClassIndent() const;
	bool getEmptyLineFill() const;
	bool getForceTabIndentation() const;
	bool getModeManuallySet() const;
	bool getModifierIndent() const;
	bool getNamespaceIndent() const;
	bool getPreprocDefineIndent() const;
	bool getSwitchIndent() const;

protected:
	void deleteBeautifierVectors();
	int  getNextProgramCharDistance(const std::string& line, int i) const;
	int  indexOf(const std::vector<const std::string*>& container, const std::string* element) const;
	void setBlockIndent(bool state);
	void setBraceIndent(bool state);
	void setBraceIndentVtk(bool state);
	std::string extractPreprocessorStatement(const std::string& line) const;
	std::string trim(const std::string& str) const;
	std::string rtrim(const std::string& str) const;

	// variables set by ASFormatter - must be updated in activeBeautifierStack
	int  inLineNumber;
	int  runInIndentContinuation;
	int  nonInStatementBrace;
	int  objCColonAlignSubsequent;		// for subsequent lines not counting indent
	bool lineCommentNoBeautify;
	bool isElseHeaderIndent;
	bool isCaseHeaderCommentIndent;
	bool isNonInStatementArray;
	bool isSharpAccessor;
	bool isSharpDelegate;
	bool isInExternC;
	bool isInBeautifySQL;
	bool isInIndentableStruct;
	bool isInIndentablePreproc;
	bool lambdaIndicator;


private:  // functions
	void adjustObjCMethodDefinitionIndentation(const std::string& line_);
	void adjustObjCMethodCallIndentation(const std::string& line_);
	void adjustParsedLineIndentation(size_t iPrelim, bool isInExtraHeaderIndent);
	void computePreliminaryIndentation();
	void parseCurrentLine(const std::string& line);
	void popLastContinuationIndent();
	void processPreprocessor(const std::string& preproc, const std::string& line);
	void registerContinuationIndent(const std::string& line, int i, int spaceIndentCount_,
	                                int tabIncrementIn, int minIndent, bool updateParenStack);
	void registerContinuationIndentColon(const std::string& line, int i, int tabIncrementIn);
	void initVectors();
	void initTempStacksContainer(std::vector<std::vector<const std::string*>*>*& container,
	                             std::vector<std::vector<const std::string*>*>* value);
	void clearObjCMethodDefinitionAlignment();
	void deleteBeautifierContainer(std::vector<ASBeautifier*>*& container);
	void deleteTempStacksContainer(std::vector<std::vector<const std::string*>*>*& container);
	int  adjustIndentCountForBreakElseIfComments() const;
	int  computeObjCColonAlignment(const std::string& line, int colonAlignPosition) const;
	int  convertTabToSpaces(int i, int tabIncrementIn) const;
	int  findObjCColonAlignment(const std::string& line) const;
	int  getContinuationIndentAssign(const std::string& line, size_t currPos) const;
	int  getContinuationIndentComma(const std::string& line, size_t currPos) const;
	int  getObjCFollowingKeyword(const std::string& line, int bracePos) const;
	bool isIndentedPreprocessor(const std::string& line, size_t currPos) const;
	bool isLineEndComment(const std::string& line, int startPos) const;
	bool isPreprocessorConditionalCplusplus(const std::string& line) const;
	bool isInPreprocessorUnterminatedComment(const std::string& line);
	bool isTopLevel() const;
	bool statementEndsWithComma(const std::string& line, int index) const;
	const std::string& getIndentedLineReturn(const std::string& newLine, const std::string& originalLine) const;
	std::string getIndentedSpaceEquivalent(const std::string& line_) const;
	std::string preLineWS(int lineIndentCount, int lineSpaceIndentCount) const;
	template<typename T> void deleteContainer(T& container);
	template<typename T> void initContainer(T& container, T value);
	std::vector<std::vector<const std::string*>*>* copyTempStacks(const ASBeautifier& other) const;
	std::pair<int, int> computePreprocessorIndent();

private:  // variables
	int beautifierFileType;
	std::vector<const std::string*>* headers;
	std::vector<const std::string*>* nonParenHeaders;
	std::vector<const std::string*>* preBlockStatements;
	std::vector<const std::string*>* preCommandHeaders;
	std::vector<const std::string*>* assignmentOperators;
	std::vector<const std::string*>* nonAssignmentOperators;
	std::vector<const std::string*>* indentableHeaders;

	std::vector<ASBeautifier*>* waitingBeautifierStack;
	std::vector<ASBeautifier*>* activeBeautifierStack;
	std::vector<size_t>* waitingBeautifierStackLengthStack;
	std::vector<size_t>* activeBeautifierStackLengthStack;
	std::vector<const std::string*>* headerStack;
	std::vector<std::vector<const std::string*>* >* tempStacks;
	std::vector<int>* parenDepthStack;
	std::vector<bool>* blockStatementStack;
	std::vector<bool>* parenStatementStack;
	std::vector<bool>* braceBlockStateStack;
	std::vector<int>* continuationIndentStack;
	std::vector<size_t>* continuationIndentStackSizeStack;
	std::vector<int>* parenIndentStack;
	std::vector<std::pair<int, int> >* preprocIndentStack;
	std::vector<std::pair<size_t, size_t> > squeezeWSStack;

	ASSourceIterator* sourceIterator;
	const std::string* currentHeader;
	const std::string* previousLastLineHeader;
	const std::string* probationHeader;
	const std::string* lastLineHeader;
	std::string indentString;
	std::string verbatimDelimiter;
	bool isInQuote;
	bool isInVerbatimQuote;
	bool haveLineContinuationChar;
	bool isInAsm;
	bool isInAsmOneLine;
	bool isInAsmBlock;
	bool isInComment;
	bool isInPreprocessorComment;
	bool isInRunInComment;
	bool isInCase;
	bool isInQuestion;
	bool isContinuation;
	bool isInHeader;
	bool isInTemplate;
	bool isInDefine;
	bool isInDefineDefinition;
	bool classIndent;
	bool isIndentModeOff;
	bool isInClassHeader;			// is in a class before the opening brace
	bool isInClassHeaderTab;		// is in an indentable class header line
	bool isInClassInitializer;		// is in a class after the ':' initializer
	bool isInClass;					// is in a class after the opening brace
	bool isInObjCMethodDefinition;
	bool isInObjCMethodCall;
	bool isInObjCMethodCallFirst;
	bool isImmediatelyPostObjCMethodDefinition;
	bool isImmediatelyPostObjCMethodCall;
	bool isInIndentablePreprocBlock;
	bool isInObjCInterface;
	bool isInEnum;
	bool isInEnumTypeID;
	bool isInStruct;
	bool isInLet;
	bool isInTrailingReturnType;
	bool modifierIndent;
	bool switchIndent;
	bool caseIndent;
	bool namespaceIndent;
	bool blockIndent;
	bool braceIndent;
	bool braceIndentVtk;
	bool shouldIndentAfterParen;
	bool labelIndent;
	bool shouldIndentPreprocDefine;
	bool isInConditional;
	bool isModeManuallySet;
	bool shouldForceTabIndentation;
	bool emptyLineFill;
	bool backslashEndsPrevLine;
	bool lineOpensWithLineComment;
	bool lineOpensWithComment;
	bool lineStartsInComment;
	bool blockCommentNoIndent;
	bool blockCommentNoBeautify;
	bool previousLineProbationTab;
	bool lineBeginsWithOpenBrace;
	bool lineBeginsWithCloseBrace;
	bool lineBeginsWithComma;
	bool lineIsCommentOnly;
	bool lineIsLineCommentOnly;
	bool shouldIndentBracedLine;
	bool isInSwitch;
	bool foundPreCommandHeader;
	bool foundPreCommandMacro;
	bool shouldAlignMethodColon;
	bool shouldIndentPreprocConditional;
	bool squeezeWhitespace;
	bool attemptLambdaIndentation;

	bool isInAssignment;

	int  indentCount;
	int  spaceIndentCount;
	int  spaceIndentObjCMethodAlignment;
	int  bracePosObjCMethodAlignment;
	int  colonIndentObjCMethodAlignment;
	int  lineOpeningBlocksNum;
	int  lineClosingBlocksNum;
	int  fileType;
	int  minConditionalOption;
	int  minConditionalIndent;
	int  parenDepth;
	int  indentLength;
	int  tabLength;
	int  continuationIndent;
	int  blockTabCount;
	int  maxContinuationIndent;
	int  classInitializerIndents;
	int  templateDepth;
	int  squareBracketCount;
	int  prevFinalLineSpaceIndentCount;
	int  prevFinalLineIndentCount;
	int  defineIndentCount;
	int  preprocBlockIndent;
	char quoteChar;
	char prevNonSpaceCh;
	char currentNonSpaceCh;
	char currentNonLegalCh;
	char prevNonLegalCh;
};  // Class ASBeautifier

//-----------------------------------------------------------------------------
// Class ASEnhancer
//-----------------------------------------------------------------------------

class ASEnhancer : protected ASBase
{
public:  // functions
	ASEnhancer() = default;
	void init(int, int, int, bool, bool, bool, bool, bool, bool, bool,
	          std::vector<const std::pair<const std::string, const std::string>* >*);
	void enhance(std::string& line, bool isInNamespace, bool isInPreprocessor, bool isInSQL);

private:  // functions
	void   convertForceTabIndentToSpaces(std::string&  line) const;
	void   convertSpaceIndentToForceTab(std::string& line) const;
	size_t findCaseColon(const std::string&  line, size_t caseIndex) const;
	int    indentLine(std::string&  line, int indent) const;
	bool   isBeginDeclareSectionSQL(const std::string&  line, size_t index) const;
	bool   isEndDeclareSectionSQL(const std::string&  line, size_t index) const;
	bool   isOneLineBlockReached(const std::string& line, int startChar) const;
	void   parseCurrentLine(std::string& line, bool isInPreprocessor, bool isInSQL);
	size_t processSwitchBlock(std::string&  line, size_t index);
	int    unindentLine(std::string&  line, int unindent) const;

private:
	// options from command line or options file
	int  indentLength;
	int  tabLength;
	bool useTabs;
	bool forceTab;
	bool namespaceIndent;
	bool caseIndent;
	bool preprocBlockIndent;
	bool preprocDefineIndent;
	bool emptyLineFill;

	// parsing variables
	int  lineNumber;
	bool isInQuote;
	bool isInComment;
	char quoteChar;

	// unindent variables
	int  braceCount;
	int  switchDepth;
	int  eventPreprocDepth;
	bool lookingForCaseBrace;
	bool unindentNextLine;
	bool shouldUnindentLine;
	bool shouldUnindentComment;

	// struct used by ParseFormattedLine function
	// contains variables used to unindent the case blocks
	struct SwitchVariables
	{
		int  switchBraceCount;
		int  unindentDepth;
		bool unindentCase;
	};

	SwitchVariables sw;                      // switch variables struct
	std::vector<SwitchVariables> switchStack;     // stack std::vector of switch variables

	// event table variables
	bool nextLineIsEventIndent;             // begin event table indent is reached
	bool isInEventTable;                    // need to indent an event table
	std::vector<const std::pair<const std::string, const std::string>* >* indentableMacros;

	// SQL variables
	bool nextLineIsDeclareIndent;           // begin declare section indent is reached
	bool isInDeclareSection;                // need to indent a declare section

};  // Class ASEnhancer

//-----------------------------------------------------------------------------
// Class ASFormatter
//-----------------------------------------------------------------------------

class ASFormatter : public ASBeautifier
{
public:	// functions
	ASFormatter();
	~ASFormatter() override;
	ASFormatter(const ASFormatter&)            = delete;
	ASFormatter& operator=(ASFormatter const&) = delete;
	ASFormatter(ASFormatter&&)                 = delete;
	ASFormatter& operator=(ASFormatter&&)      = delete;
	void init(ASSourceIterator* si) override;

	bool hasMoreLines() const;
	std::string nextLine();
	LineEndFormat getLineEndFormat() const;
	bool getIsLineReady() const;
	void setFormattingStyle(FormatStyle style);
	void setAddBracesMode(bool state);
	void setAddOneLineBracesMode(bool state);
	void setRemoveBracesMode(bool state);
	void setAttachClass(bool state);
	void setAttachClosingWhile(bool state);
	void setAttachExternC(bool state);
	void setAttachNamespace(bool state);
	void setAttachInline(bool state);
	void setBraceFormatMode(BraceMode mode);
	void setBreakAfterMode(bool state);
	void setBreakClosingHeaderBracesMode(bool state);
	void setBreakBlocksMode(bool state);
	void setBreakClosingHeaderBlocksMode(bool state);
	void setBreakElseIfsMode(bool state);
	void setBreakOneLineBlocksMode(bool state);
	void setBreakOneLineHeadersMode(bool state);
	void setBreakOneLineStatementsMode(bool state);
	void setMethodPrefixPaddingMode(bool state);
	void setMethodPrefixUnPaddingMode(bool state);
	void setReturnTypePaddingMode(bool state);
	void setReturnTypeUnPaddingMode(bool state);
	void setParamTypePaddingMode(bool state);
	void setParamTypeUnPaddingMode(bool state);
	void setCloseTemplatesMode(bool state);
	void setCommaPaddingMode(bool state);
	void setDeleteEmptyLinesMode(bool state);
	void setBreakReturnType(bool state);
	void setBreakReturnTypeDecl(bool state);
	void setAttachReturnType(bool state);
	void setAttachReturnTypeDecl(bool state);
	void setIndentCol1CommentsMode(bool state);
	void setLineEndFormat(LineEndFormat fmt);
	void setMaxCodeLength(int max);
	void setObjCColonPaddingMode(ObjCColonPad mode);
	void setOperatorPaddingMode(bool state);
	void setParensOutsidePaddingMode(bool state);
	void setParensFirstPaddingMode(bool state);

	void setEmptyParensPaddingMode(bool state);

	void setParensInsidePaddingMode(bool state);
	void setParensHeaderPaddingMode(bool state);
	void setParensUnPaddingMode(bool state);

	void setBracketsOutsidePaddingMode(bool state);
	void setBracketsInsidePaddingMode(bool state);
	void setBracketsUnPaddingMode(bool state);

	void setPointerAlignment(PointerAlign alignment);
	void setPreprocBlockIndent(bool state);
	void setReferenceAlignment(ReferenceAlign alignment);
	void setStripCommentPrefix(bool state);
	void setTabSpaceConversionMode(bool state);
	size_t getChecksumIn() const;
	size_t getChecksumOut() const;
	int  getChecksumDiff() const;
	int  getFormatterFileType() const;
	// retained for compatibility with release 2.06
	// "Brackets" have been changed to "Braces" in 3.0
	// they are referenced only by the old "bracket" options
	void setAddBracketsMode(bool state);
	void setAddOneLineBracketsMode(bool state);
	void setRemoveBracketsMode(bool state);
	void setBreakClosingHeaderBracketsMode(bool state);
	void setSqueezeEmptyLinesNumber(int);

private:  // functions
	template<typename T> void deleteContainer(T& container);
	template<typename T> void initContainer(T& container, T value);
	char peekNextChar() const;
	BraceType getBraceType();
	bool adjustChecksumIn(int adjustment);
	bool computeChecksumIn(const std::string& currentLine_);
	bool computeChecksumOut(const std::string& beautifiedLine);
	bool addBracesToStatement();
	bool removeBracesFromStatement();
	bool commentAndHeaderFollows();
	bool getNextChar();
	bool getNextLine(bool emptyLineWasDeleted = false);
	bool isArrayOperator() const;
	bool isBeforeComment() const;
	bool isBeforeAnyComment() const;
	bool isBeforeAnyLineEndComment(int startPos) const;
	bool isBeforeMultipleLineEndComments(int startPos) const;
	bool isBraceType(BraceType a, BraceType b) const;
	bool isClassInitializer() const;
	bool isClosingHeader(const std::string* header) const;
	bool isCurrentBraceBroken() const;
	bool isDereferenceOrAddressOf() const;
	bool isExecSQL(const std::string& line, size_t index) const;
	bool isEmptyLine(const std::string& line) const;
	bool isExternC() const;
	bool isMultiStatementLine() const;
	bool isNextWordSharpNonParenHeader(int startChar) const;
	bool isNonInStatementArrayBrace() const;
	bool isNumericVariable(const std::string& word) const;
	bool isOkToSplitFormattedLine();
	bool isPointerOrReference() const;
	bool isPointerOrReferenceCentered() const;
	bool isPointerOrReferenceVariable(const std::string& word) const;
	bool isPointerToPointer(const std::string& line, int currPos) const;
	bool isSharpStyleWithParen(const std::string* header) const;
	bool isStructAccessModified(const std::string& firstLine, size_t index) const;
	bool isIndentablePreprocessorBlock(const std::string& firstLine, size_t index);
	bool isNDefPreprocStatement(const std::string& nextLine_, const std::string& preproc) const;
	bool isUnaryOperator() const;
	bool isUniformInitializerBrace() const;
	bool isImmediatelyPostCast() const;
	bool isInExponent() const;
	bool isInSwitchStatement() const;
	bool isNextCharOpeningBrace(int startChar) const;
	bool isOkToBreakBlock(BraceType braceType) const;
	bool isOperatorPaddingDisabled() const;
	bool pointerSymbolFollows() const;
	int  findObjCColonAlignment() const;
	int  getCurrentLineCommentAdjustment();
	int  getNextLineCommentAdjustment();
	int  isOneLineBlockReached(const std::string& line, int startChar) const;
	void adjustComments();
	void appendChar(char ch, bool canBreakLine);
	void appendCharInsideComments();
	void appendClosingHeader();
	void appendOperator(const std::string& sequence, bool canBreakLine = true);
	void appendSequence(const std::string& sequence, bool canBreakLine = true);
	void appendSpacePad();
	void appendSpaceAfter();
	void breakLine(bool isSplitLine = false);
	void buildLanguageVectors();
	void updateFormattedLineSplitPoints(char appendedChar);
	void updateFormattedLineSplitPointsOperator(const std::string& sequence);
	void checkIfTemplateOpener();
	void clearFormattedLineSplitPoints();
	void convertTabToSpaces();
	void deleteContainer(std::vector<BraceType>*& container);
	void findReturnTypeSplitPoint(const std::string& firstLine);
	void formatArrayRunIn();
	void formatRunIn();
	void formatArrayBraces(BraceType braceType, bool isOpeningArrayBrace);
	void formatClosingBrace(BraceType braceType);
	void formatCommentBody();
	void formatCommentOpener();
	void formatCommentCloser();
	void formatLineCommentBody();
	void formatLineCommentOpener();
	void formatOpeningBrace(BraceType braceType);
	void formatQuoteBody();
	void formatQuoteOpener();
	void formatPointerOrReference();
	void formatPointerOrReferenceCast();
	void formatPointerOrReferenceToMiddle();
	void formatPointerOrReferenceToName();
	void formatPointerOrReferenceToType();
	void fixOptionVariableConflicts();
	void goForward(int i);
	void isLineBreakBeforeClosingHeader();
	void initContainer(std::vector<BraceType>*& container, std::vector<BraceType>* value);
	void initNewLine();
	void padObjCMethodColon();
	void padObjCMethodPrefix();
	void padObjCParamType();
	void padObjCReturnType();
	void padOperators(const std::string* newOperator);
	void padParensOrBrackets(char openDelim, char closeDelim, bool shouldPadParensOutside, bool shouldPadParensInside, bool shouldUnPadParens, bool shouldPadFirstParen);
	void processPreprocessor();
	void resetEndOfStatement();
	void setAttachClosingBraceMode(bool state);
	void stripCommentPrefix();
	void testForTimeToSplitFormattedLine();
	void trimContinuationLine();
	void updateFormattedLineSplitPointsPointerOrReference(size_t index);
	size_t findFormattedLineSplitPoint() const;
	size_t findNextChar(const std::string& line, char searchChar, int searchStart = 0) const;
	const std::string* checkForHeaderFollowingComment(const std::string& firstLine) const;
	const std::string* getFollowingOperator() const;
	std::string getPreviousWord(const std::string& line, int currPos) const;
	std::string peekNextText(const std::string& firstLine,
	                         bool endOnEmptyLine = false,
	                         const std::shared_ptr<ASPeekStream>& streamArg = nullptr) const;

private:  // variables
	int formatterFileType;
	std::vector<const std::string*>* headers;
	std::vector<const std::string*>* nonParenHeaders;
	std::vector<const std::string*>* preDefinitionHeaders;
	std::vector<const std::string*>* preCommandHeaders;
	std::vector<const std::string*>* operators;
	std::vector<const std::string*>* assignmentOperators;
	std::vector<const std::string*>* castOperators;
	std::vector<const std::pair<const std::string, const std::string>* >* indentableMacros;	// for ASEnhancer

	ASSourceIterator* sourceIterator;
	ASEnhancer* enhancer;

	std::vector<const std::string*>* preBraceHeaderStack;
	std::vector<BraceType>* braceTypeStack;
	std::vector<int>* parenStack;
	std::vector<bool>* structStack;
	std::vector<bool>* questionMarkStack;

	std::string currentLine;
	std::string formattedLine;
	std::string readyFormattedLine;
	std::string verbatimDelimiter;
	const std::string* currentHeader;
	char currentChar;
	char previousChar;
	char previousNonWSChar;
	char previousCommandChar;
	char quoteChar;
	std::streamoff preprocBlockEnd;
	int  charNum;
	int  runInIndentChars;
	int  nextLineSpacePadNum;
	int  objCColonAlign;
	int  preprocBraceTypeStackSize;
	int  spacePadNum;
	int  tabIncrementIn;
	int  templateDepth;
	int  squareBracketCount;
	int  parenthesesCount;
	int  squeezeEmptyLineNum;
	int  squeezeEmptyLineCount;

	size_t checksumIn;
	size_t checksumOut;
	size_t currentLineFirstBraceNum;	// first brace location on currentLine
	size_t formattedLineCommentNum;     // comment location on formattedLine
	size_t leadingSpaces;
	size_t maxCodeLength;
	size_t methodAttachCharNum;
	size_t methodAttachLineNum;
	size_t methodBreakCharNum;
	size_t methodBreakLineNum;

	// possible split points
	size_t maxSemi;			// probably a 'for' statement
	size_t maxAndOr;		// probably an 'if' statement
	size_t maxComma;
	size_t maxParen;
	size_t maxWhiteSpace;
	size_t maxSemiPending;
	size_t maxAndOrPending;
	size_t maxCommaPending;
	size_t maxParenPending;
	size_t maxWhiteSpacePending;

	size_t previousReadyFormattedLineLength;
	FormatStyle formattingStyle;
	BraceMode braceFormatMode;
	BraceType previousBraceType;
	PointerAlign pointerAlignment;
	ReferenceAlign referenceAlignment;
	ObjCColonPad objCColonPadMode;
	LineEndFormat lineEnd;
	bool isVirgin;
	bool isInVirginLine;
	bool shouldPadCommas;
	bool shouldPadOperators;
	bool shouldPadParensOutside;
	bool shouldPadFirstParen;

	bool shouldPadEmptyParens;
	bool shouldPadParensInside;
	bool shouldPadHeader;
	bool shouldStripCommentPrefix;
	bool shouldUnPadParens;
	bool shouldConvertTabs;
	bool shouldIndentCol1Comments;
	bool shouldIndentPreprocBlock;
	bool shouldCloseTemplates;
	bool shouldAttachExternC;
	bool shouldAttachNamespace;
	bool shouldAttachClass;
	bool shouldAttachClosingWhile;
	bool shouldAttachInline;
	bool isInLineComment;
	bool isInComment;
	bool isInCommentStartLine;
	bool noTrimCommentContinuation;
	bool isInPreprocessor;
	bool isInPreprocessorDefineDef;
	bool isInPreprocessorBeautify;
	bool isInTemplate;
	bool doesLineStartComment;
	bool lineEndsInCommentOnly;
	bool lineIsCommentOnly;
	bool lineIsLineCommentOnly;
	bool lineIsEmpty;
	bool isImmediatelyPostCommentOnly;
	bool isImmediatelyPostEmptyLine;
	bool isInClassInitializer;
	bool isInQuote;
	bool isInVerbatimQuote;
	bool checkInterpolation;
	bool haveLineContinuationChar;
	bool isInQuoteContinuation;
	bool isHeaderInMultiStatementLine;
	bool isSpecialChar;
	bool isNonParenHeader;
	bool foundQuestionMark;
	bool foundPreDefinitionHeader;
	bool foundNamespaceHeader;
	bool foundClassHeader;
	bool foundStructHeader;
	bool foundInterfaceHeader;
	bool foundPreCommandHeader;
	bool foundPreCommandMacro;
	bool foundTrailingReturnType;
	bool foundCastOperator;
	bool isInLineBreak;
	bool endOfAsmReached;
	bool endOfCodeReached;
	bool lineCommentNoIndent;
	bool isFormattingModeOff;
	bool isInEnum;
	bool isInStruct;
	bool isInContinuedPreProc;
	bool isInExecSQL;
	bool isInAsm;
	bool isInAsmOneLine;
	bool isInAsmBlock;
	bool isLineReady;
	bool elseHeaderFollowsComments;
	bool caseHeaderFollowsComments;
	bool isPreviousBraceBlockRelated;
	bool isInPotentialCalculation;
	bool isCharImmediatelyPostComment;
	bool isPreviousCharPostComment;
	bool isCharImmediatelyPostLineComment;
	bool isCharImmediatelyPostOpenBlock;
	bool isCharImmediatelyPostCloseBlock;
	bool isCharImmediatelyPostTemplate;
	bool isCharImmediatelyPostReturn;
	bool isCharImmediatelyPostThrow;
	bool isCharImmediatelyPostNewDelete;
	bool isCharImmediatelyPostOperator;
	bool isCharImmediatelyPostPointerOrReference;
	bool isInObjCMethodDefinition;
	bool isInObjCInterface;
	bool isInObjCReturnType;
	bool isInObjCParam;
	bool isInObjCSelector;
	bool breakCurrentOneLineBlock;
	bool shouldRemoveNextClosingBrace;
	bool isInBraceRunIn;
	bool returnTypeChecked;
	bool currentLineBeginsWithBrace;
	bool attachClosingBraceMode;
	bool shouldBreakOneLineBlocks;
	bool shouldBreakOneLineHeaders;
	bool shouldBreakOneLineStatements;
	bool shouldBreakClosingHeaderBraces;
	bool shouldBreakElseIfs;
	bool shouldBreakLineAfterLogical;
	bool shouldAddBraces;
	bool shouldAddOneLineBraces;
	bool shouldRemoveBraces;
	bool shouldPadMethodColon;
	bool shouldPadMethodPrefix;
	bool shouldReparseCurrentChar;
	bool shouldUnPadMethodPrefix;
	bool shouldPadReturnType;
	bool shouldUnPadReturnType;
	bool shouldPadParamType;
	bool shouldUnPadParamType;
	bool shouldDeleteEmptyLines;
	bool shouldBreakReturnType;
	bool shouldBreakReturnTypeDecl;
	bool shouldAttachReturnType;
	bool shouldAttachReturnTypeDecl;
	bool shouldPadBracketsOutside;
	bool shouldPadBracketsInside;
	bool shouldUnPadBrackets;
	bool needHeaderOpeningBrace;
	bool shouldBreakLineAtNextChar;
	bool shouldKeepLineUnbroken;
	bool passedSemicolon;
	bool passedColon;
	bool isImmediatelyPostNonInStmt;
	bool isCharImmediatelyPostNonInStmt;
	bool isImmediatelyPostComment;
	bool isImmediatelyPostLineComment;
	bool isImmediatelyPostEmptyBlock;
	bool isImmediatelyPostObjCMethodPrefix;
	bool isImmediatelyPostPreprocessor;
	bool isImmediatelyPostReturn;
	bool isImmediatelyPostThrow;
	bool isImmediatelyPostNewDelete;
	bool isImmediatelyPostOperator;
	bool isImmediatelyPostTemplate;
	bool isImmediatelyPostPointerOrReference;
	bool shouldBreakBlocks;
	bool shouldBreakClosingHeaderBlocks;
	bool isPrependPostBlockEmptyLineRequested;
	bool isAppendPostBlockEmptyLineRequested;
	bool isIndentablePreprocessor;
	bool isIndentablePreprocessorBlck;
	bool prependEmptyLine;
	bool appendOpeningBrace;
	bool foundClosingHeader;
	bool isInHeader;
	bool isImmediatelyPostHeader;
	bool isInCase;
	bool isFirstPreprocConditional;
	bool processedFirstConditional;
	bool isJavaStaticConstructor;
	bool isInAllocator;

private:  // inline functions
	// append the CURRENT character (curentChar) to the current formatted line.
	void appendCurrentChar(bool canBreakLine = true)
	{ appendChar(currentChar, canBreakLine); }

	// check if a specific sequence exists in the current placement of the current line
	bool isSequenceReached(const char* sequence) const
	{ return currentLine.compare(charNum, strlen(sequence), sequence) == 0; }

	// call ASBase::findHeader for the current character
	const std::string* findHeader(const std::vector<const std::string*>* headers_)
	{ return ASBase::findHeader(currentLine, charNum, headers_); }

	// call ASBase::findOperator for the current character
	const std::string* findOperator(const std::vector<const std::string*>* operators_)
	{ return ASBase::findOperator(currentLine, charNum, operators_); }
};  // Class ASFormatter

//-----------------------------------------------------------------------------
// astyle namespace global declarations
//-----------------------------------------------------------------------------
// sort comparison functions for ASResource
bool sortOnLength(const std::string* a, const std::string* b);
bool sortOnName(const std::string* a, const std::string* b);

}   // namespace astyle

// end of astyle namespace  --------------------------------------------------

#endif // closes ASTYLE_H
