// ASResource.cpp
// Copyright (c) 2023 The Artistic Style Authors.
// This code is licensed under the MIT License.
// License.md describes the conditions under which this software may be distributed.

//-----------------------------------------------------------------------------
// headers
//-----------------------------------------------------------------------------

#include "astyle.h"
#include <algorithm>

//-----------------------------------------------------------------------------
// astyle namespace
//-----------------------------------------------------------------------------

namespace astyle {
//
const std::string ASResource::_AS_EXCEPT = std::string("__except");
const std::string ASResource::_AS_FINALLY = std::string("__finally");
const std::string ASResource::_AS_TRY = std::string("__try");
const std::string ASResource::AS_ADD = std::string("add");
const std::string ASResource::AS_AUTO = std::string("auto");
const std::string ASResource::AS_AUTORELEASEPOOL = std::string("autoreleasepool");
const std::string ASResource::AS_CASE = std::string("case");
const std::string ASResource::AS_CATCH = std::string("catch");
const std::string ASResource::AS_CLASS = std::string("class");
const std::string ASResource::AS_CONST = std::string("const");
const std::string ASResource::AS_CONST_CAST = std::string("const_cast");
const std::string ASResource::AS_DEFAULT = std::string("default");
const std::string ASResource::AS_DELEGATE = std::string("delegate");
const std::string ASResource::AS_DELETE = std::string("delete");
const std::string ASResource::AS_DO = std::string("do");
const std::string ASResource::AS_DYNAMIC_CAST = std::string("dynamic_cast");
const std::string ASResource::AS_ELSE = std::string("else");
const std::string ASResource::AS_END = std::string("end");
const std::string ASResource::AS_ENUM = std::string("enum");
const std::string ASResource::AS_EXTERN = std::string("extern");
const std::string ASResource::AS_FINAL = std::string("final");
const std::string ASResource::AS_FINALLY = std::string("finally");
const std::string ASResource::AS_FIXED = std::string("fixed");
const std::string ASResource::AS_FOR = std::string("for");
const std::string ASResource::AS_FOREACH = std::string("foreach");
const std::string ASResource::AS_FOREVER = std::string("forever");
const std::string ASResource::AS_GET = std::string("get");
const std::string ASResource::AS_IF = std::string("if");
const std::string ASResource::AS_INTERFACE = std::string("interface");
const std::string ASResource::AS_INTERRUPT = std::string("interrupt");
const std::string ASResource::AS_LET = std::string("let");
const std::string ASResource::AS_LOCK = std::string("lock");
const std::string ASResource::AS_MODULE = std::string("module");	// CORBA IDL module definition
const std::string ASResource::AS_NAMESPACE = std::string("namespace");
const std::string ASResource::AS_NEW = std::string("new");
const std::string ASResource::AS_NOEXCEPT = std::string("noexcept");
const std::string ASResource::AS_NS_DURING = std::string("NS_DURING");
const std::string ASResource::AS_NS_HANDLER = std::string("NS_HANDLER");
const std::string ASResource::AS_OPERATOR = std::string("operator");
const std::string ASResource::AS_OVERRIDE = std::string("override");
const std::string ASResource::AS_PRIVATE = std::string("private");
const std::string ASResource::AS_PROTECTED = std::string("protected");
const std::string ASResource::AS_PUBLIC = std::string("public");
const std::string ASResource::AS_QFOREACH = std::string("Q_FOREACH");
const std::string ASResource::AS_QFOREVER = std::string("Q_FOREVER");
const std::string ASResource::AS_REINTERPRET_CAST = std::string("reinterpret_cast");
const std::string ASResource::AS_REMOVE = std::string("remove");
const std::string ASResource::AS_SEALED = std::string("sealed");
const std::string ASResource::AS_SELECTOR = std::string("selector");
const std::string ASResource::AS_SET = std::string("set");
const std::string ASResource::AS_STATIC = std::string("static");
const std::string ASResource::AS_STATIC_CAST = std::string("static_cast");
const std::string ASResource::AS_STRUCT = std::string("struct");
const std::string ASResource::AS_TYPEDEF_STRUCT = std::string("typedef struct");
const std::string ASResource::AS_SWITCH = std::string("switch");
const std::string ASResource::AS_SYNCHRONIZED = std::string("synchronized");
const std::string ASResource::AS_TEMPLATE = std::string("template");
const std::string ASResource::AS_THROW = std::string("throw");
const std::string ASResource::AS_THROWS = std::string("throws");
const std::string ASResource::AS_TRY = std::string("try");
const std::string ASResource::AS_UNCHECKED = std::string("unchecked");
const std::string ASResource::AS_UNION = std::string("union");
const std::string ASResource::AS_UNSAFE = std::string("unsafe");
const std::string ASResource::AS_USING = std::string("using");
const std::string ASResource::AS_VOLATILE = std::string("volatile");
const std::string ASResource::AS_WHERE = std::string("where");
const std::string ASResource::AS_WHILE = std::string("while");

const std::string ASResource::AS_ASM = std::string("asm");
const std::string ASResource::AS__ASM__ = std::string("__asm__");
const std::string ASResource::AS_MS_ASM = std::string("_asm");
const std::string ASResource::AS_MS__ASM = std::string("__asm");

const std::string ASResource::AS_BAR_DEFINE = std::string("#define");
const std::string ASResource::AS_BAR_INCLUDE = std::string("#include");
const std::string ASResource::AS_BAR_IF = std::string("#if");
const std::string ASResource::AS_BAR_EL = std::string("#el");
const std::string ASResource::AS_BAR_ENDIF = std::string("#endif");

const std::string ASResource::AS_OPEN_PAREN = std::string("(");
const std::string ASResource::AS_CLOSE_PAREN = std::string(")");
const std::string ASResource::AS_OPEN_BRACE = std::string("{");
const std::string ASResource::AS_CLOSE_BRACE = std::string("}");
const std::string ASResource::AS_OPEN_LINE_COMMENT = std::string("//");
const std::string ASResource::AS_OPEN_COMMENT = std::string("/*");
const std::string ASResource::AS_CLOSE_COMMENT = std::string("*/");

const std::string ASResource::AS_ASSIGN = std::string("=");
const std::string ASResource::AS_PLUS_ASSIGN = std::string("+=");
const std::string ASResource::AS_MINUS_ASSIGN = std::string("-=");
const std::string ASResource::AS_MULT_ASSIGN = std::string("*=");
const std::string ASResource::AS_DIV_ASSIGN = std::string("/=");
const std::string ASResource::AS_MOD_ASSIGN = std::string("%=");
const std::string ASResource::AS_OR_ASSIGN = std::string("|=");
const std::string ASResource::AS_AND_ASSIGN = std::string("&=");
const std::string ASResource::AS_XOR_ASSIGN = std::string("^=");
const std::string ASResource::AS_GR_GR_ASSIGN = std::string(">>=");
const std::string ASResource::AS_LS_LS_ASSIGN = std::string("<<=");
const std::string ASResource::AS_GR_GR_GR_ASSIGN = std::string(">>>=");
const std::string ASResource::AS_LS_LS_LS_ASSIGN = std::string("<<<=");
const std::string ASResource::AS_GCC_MIN_ASSIGN = std::string("<?");
const std::string ASResource::AS_GCC_MAX_ASSIGN = std::string(">?");

const std::string ASResource::AS_RETURN = std::string("return");
const std::string ASResource::AS_CIN = std::string("cin");
const std::string ASResource::AS_COUT = std::string("cout");
const std::string ASResource::AS_CERR = std::string("cerr");

const std::string ASResource::AS_EQUAL = std::string("==");
const std::string ASResource::AS_PLUS_PLUS = std::string("++");
const std::string ASResource::AS_MINUS_MINUS = std::string("--");
const std::string ASResource::AS_NOT_EQUAL = std::string("!=");
const std::string ASResource::AS_GR_EQUAL = std::string(">=");
const std::string ASResource::AS_GR_GR = std::string(">>");
const std::string ASResource::AS_GR_GR_GR = std::string(">>>");
const std::string ASResource::AS_LS_EQUAL = std::string("<=");
const std::string ASResource::AS_LS_LS = std::string("<<");
const std::string ASResource::AS_LS_LS_LS = std::string("<<<");
const std::string ASResource::AS_QUESTION_QUESTION = std::string("??");
const std::string ASResource::AS_LAMBDA = std::string("=>");            // C# lambda expression arrow
const std::string ASResource::AS_ARROW = std::string("->");
const std::string ASResource::AS_AND = std::string("&&");
const std::string ASResource::AS_OR = std::string("||");
const std::string ASResource::AS_SCOPE_RESOLUTION = std::string("::");
const std::string ASResource::AS_SPACESHIP = std::string("<=>");
const std::string ASResource::AS_EQUAL_JS = std::string("===");
const std::string ASResource::AS_COALESCE_CS = std::string("?" "?="); // Avoid trigraph

const std::string ASResource::AS_PLUS = std::string("+");
const std::string ASResource::AS_MINUS = std::string("-");
const std::string ASResource::AS_MULT = std::string("*");
const std::string ASResource::AS_DIV = std::string("/");
const std::string ASResource::AS_MOD = std::string("%");
const std::string ASResource::AS_GR = std::string(">");
const std::string ASResource::AS_LS = std::string("<");
const std::string ASResource::AS_NOT = std::string("!");
const std::string ASResource::AS_BIT_OR = std::string("|");
const std::string ASResource::AS_BIT_AND = std::string("&");
const std::string ASResource::AS_BIT_NOT = std::string("~");
const std::string ASResource::AS_BIT_XOR = std::string("^");
const std::string ASResource::AS_QUESTION = std::string("?");
const std::string ASResource::AS_COLON = std::string(":");
const std::string ASResource::AS_COMMA = std::string(",");
const std::string ASResource::AS_DOT = std::string(".");
const std::string ASResource::AS_SEMICOLON = std::string(";");


/**
 * Sort comparison function.
 * Compares the length of the value of pointers in the vectors.
 * The LONGEST std::strings will be first in the vector.
 *
 * @param a and b, the std::string pointers to be compared.
 */
bool sortOnLength(const std::string* a, const std::string* b)
{
	return (*a).length() > (*b).length();
}

/**
 * Sort comparison function.
 * Compares the value of pointers in the vectors.
 *
 * @param a and b, the std::string pointers to be compared.
 */
bool sortOnName(const std::string* a, const std::string* b)
{
	return *a < *b;
}

/**
 * Build the vector of assignment operators.
 * Used by BOTH ASFormatter.cpp and ASBeautifier.cpp
 *
 * @param assignmentOperators   a reference to the vector to be built.
 */
void ASResource::buildAssignmentOperators(std::vector<const std::string*>* assignmentOperators)
{
	const size_t elements = 15;
	assignmentOperators->reserve(elements);

	assignmentOperators->emplace_back(&AS_ASSIGN);
	assignmentOperators->emplace_back(&AS_PLUS_ASSIGN);
	assignmentOperators->emplace_back(&AS_MINUS_ASSIGN);
	assignmentOperators->emplace_back(&AS_MULT_ASSIGN);
	assignmentOperators->emplace_back(&AS_DIV_ASSIGN);
	assignmentOperators->emplace_back(&AS_MOD_ASSIGN);
	assignmentOperators->emplace_back(&AS_OR_ASSIGN);
	assignmentOperators->emplace_back(&AS_AND_ASSIGN);
	assignmentOperators->emplace_back(&AS_XOR_ASSIGN);

	// Java
	assignmentOperators->emplace_back(&AS_GR_GR_GR_ASSIGN);
	assignmentOperators->emplace_back(&AS_GR_GR_ASSIGN);
	assignmentOperators->emplace_back(&AS_LS_LS_ASSIGN);

	// Unknown
	assignmentOperators->emplace_back(&AS_LS_LS_LS_ASSIGN);

	assert(assignmentOperators->size() < elements);
	sort(assignmentOperators->begin(), assignmentOperators->end(), sortOnLength);
}

/**
 * Build the vector of C++ cast operators.
 * Used by ONLY ASFormatter.cpp
 *
 * @param castOperators     a reference to the vector to be built.
 */
void ASResource::buildCastOperators(std::vector<const std::string*>* castOperators)
{
	const size_t elements = 5;
	castOperators->reserve(elements);

	castOperators->emplace_back(&AS_CONST_CAST);
	castOperators->emplace_back(&AS_DYNAMIC_CAST);
	castOperators->emplace_back(&AS_REINTERPRET_CAST);
	castOperators->emplace_back(&AS_STATIC_CAST);

	assert(castOperators->size() < elements);
	sort(castOperators->begin(), castOperators->end(), sortOnName);
}

/**
 * Build the vector of header words.
 * Used by BOTH ASFormatter.cpp and ASBeautifier.cpp
 *
 * @param headers       a reference to the vector to be built.
 */
void ASResource::buildHeaders(std::vector<const std::string*>* headers, int fileType, bool beautifier)
{
	const size_t elements = 25;
	headers->reserve(elements);

	headers->emplace_back(&AS_IF);
	headers->emplace_back(&AS_ELSE);
	headers->emplace_back(&AS_FOR);
	headers->emplace_back(&AS_WHILE);
	headers->emplace_back(&AS_DO);
	headers->emplace_back(&AS_SWITCH);
	headers->emplace_back(&AS_CASE);
	headers->emplace_back(&AS_DEFAULT);
	headers->emplace_back(&AS_TRY);
	headers->emplace_back(&AS_CATCH);
	headers->emplace_back(&AS_QFOREACH);		// QT
	headers->emplace_back(&AS_QFOREVER);		// QT
	headers->emplace_back(&AS_FOREACH);		// QT & C#
	headers->emplace_back(&AS_FOREVER);		// Qt & Boost

	if (fileType == C_TYPE || fileType == OBJC_TYPE)
	{
		headers->emplace_back(&_AS_TRY);		// __try
		headers->emplace_back(&_AS_FINALLY);	// __finally
		headers->emplace_back(&_AS_EXCEPT);	// __except
	}
	if (fileType == JAVA_TYPE)
	{
		headers->emplace_back(&AS_FINALLY);
		headers->emplace_back(&AS_SYNCHRONIZED);
	}

	if (fileType == SHARP_TYPE)
	{
		headers->emplace_back(&AS_FINALLY);
		headers->emplace_back(&AS_LOCK);
		headers->emplace_back(&AS_FIXED);
		headers->emplace_back(&AS_GET);
		headers->emplace_back(&AS_SET);
		headers->emplace_back(&AS_ADD);
		headers->emplace_back(&AS_REMOVE);
		headers->emplace_back(&AS_USING);
	}

	if (beautifier)
	{
		if (fileType == C_TYPE || fileType == OBJC_TYPE)
		{
			headers->emplace_back(&AS_TEMPLATE);
		}

		if (fileType == JAVA_TYPE)
		{
			headers->emplace_back(&AS_STATIC);         // for static constructor
		}
	}

	assert(headers->size() < elements);
	sort(headers->begin(), headers->end(), sortOnName);
}

/**
 * Build the vector of indentable headers.
 * Used by ONLY ASBeautifier.cpp
 *
 * @param indentableHeaders     a reference to the vector to be built.
 */
void ASResource::buildIndentableHeaders(std::vector<const std::string*>* indentableHeaders)
{
	indentableHeaders->emplace_back(&AS_RETURN);
}

/**
* Build the vector of indentable macros pairs.
* Initialized by ASFormatter, used by ONLY ASEnhancer.cpp
*
* @param indentableMacros       a reference to the vector to be built.
*/
void ASResource::buildIndentableMacros(std::vector<const std::pair<const std::string, const std::string>* >* indentableMacros)
{
	const size_t elements = 10;
	indentableMacros->reserve(elements);

	// the pairs must be retained in memory because of pair pointers
	using macro_pair = std::pair<const std::string, const std::string>;
	static const macro_pair macros[] =
	{
		// wxWidgets
		macro_pair("BEGIN_EVENT_TABLE",   "END_EVENT_TABLE"),
		macro_pair("wxBEGIN_EVENT_TABLE", "wxEND_EVENT_TABLE"),
		// MFC
		macro_pair("BEGIN_DISPATCH_MAP",  "END_DISPATCH_MAP"),
		macro_pair("BEGIN_EVENT_MAP",     "END_EVENT_MAP"),
		macro_pair("BEGIN_MESSAGE_MAP",   "END_MESSAGE_MAP"),
		macro_pair("BEGIN_PROPPAGEIDS",   "END_PROPPAGEIDS"),
	};

	for (const macro_pair& macro : macros)
		indentableMacros->emplace_back(&macro);

	assert(indentableMacros->size() < elements);
}

/**
 * Build the vector of non-assignment operators.
 * Used by ONLY ASBeautifier.cpp
 *
 * @param nonAssignmentOperators       a reference to the vector to be built.
 */
void ASResource::buildNonAssignmentOperators(std::vector<const std::string*>* nonAssignmentOperators)
{
	const size_t elements = 16;
	nonAssignmentOperators->reserve(elements);

	nonAssignmentOperators->emplace_back(&AS_EQUAL);
	nonAssignmentOperators->emplace_back(&AS_PLUS_PLUS);
	nonAssignmentOperators->emplace_back(&AS_MINUS_MINUS);
	nonAssignmentOperators->emplace_back(&AS_NOT_EQUAL);
	nonAssignmentOperators->emplace_back(&AS_GR_EQUAL);
	nonAssignmentOperators->emplace_back(&AS_GR_GR_GR);
	nonAssignmentOperators->emplace_back(&AS_GR_GR);
	nonAssignmentOperators->emplace_back(&AS_LS_EQUAL);
	nonAssignmentOperators->emplace_back(&AS_LS_LS_LS);
	nonAssignmentOperators->emplace_back(&AS_LS_LS);
	nonAssignmentOperators->emplace_back(&AS_ARROW);
	nonAssignmentOperators->emplace_back(&AS_AND);
	nonAssignmentOperators->emplace_back(&AS_OR);
	nonAssignmentOperators->emplace_back(&AS_LAMBDA);
	nonAssignmentOperators->emplace_back(&AS_DOT);

	assert(nonAssignmentOperators->size() < elements);
	sort(nonAssignmentOperators->begin(), nonAssignmentOperators->end(), sortOnLength);
}

/**
 * Build the vector of header non-paren headers.
 * Used by BOTH ASFormatter.cpp and ASBeautifier.cpp.
 * NOTE: Non-paren headers should also be included in the headers vector.
 *
 * @param nonParenHeaders       a reference to the std::vector to be built.
 */
void ASResource::buildNonParenHeaders(std::vector<const std::string*>* nonParenHeaders, int fileType, bool beautifier)
{
	const size_t elements = 20;
	nonParenHeaders->reserve(elements);

	nonParenHeaders->emplace_back(&AS_ELSE);
	nonParenHeaders->emplace_back(&AS_DO);
	nonParenHeaders->emplace_back(&AS_TRY);
	nonParenHeaders->emplace_back(&AS_CATCH);		// can be paren or non-paren
	nonParenHeaders->emplace_back(&AS_CASE);		// can be paren or non-paren
	nonParenHeaders->emplace_back(&AS_DEFAULT);
	nonParenHeaders->emplace_back(&AS_QFOREVER);	// QT
	nonParenHeaders->emplace_back(&AS_FOREVER);	// Boost

	if (fileType == C_TYPE || fileType == OBJC_TYPE)
	{
		nonParenHeaders->emplace_back(&_AS_TRY);		// __try
		nonParenHeaders->emplace_back(&_AS_FINALLY);	// __finally
	}
	if (fileType == JAVA_TYPE)
	{
		nonParenHeaders->emplace_back(&AS_FINALLY);
	}

	if (fileType == SHARP_TYPE)
	{
		nonParenHeaders->emplace_back(&AS_FINALLY);
		nonParenHeaders->emplace_back(&AS_GET);
		nonParenHeaders->emplace_back(&AS_SET);
		nonParenHeaders->emplace_back(&AS_ADD);
		nonParenHeaders->emplace_back(&AS_REMOVE);
	}

	if (beautifier)
	{
		if (fileType == C_TYPE)
		{
			nonParenHeaders->emplace_back(&AS_TEMPLATE);
		}
		if (fileType == JAVA_TYPE)
		{
			nonParenHeaders->emplace_back(&AS_STATIC);
		}
	}

	assert(nonParenHeaders->size() < elements);
	sort(nonParenHeaders->begin(), nonParenHeaders->end(), sortOnName);
}

/**
 * Build the vector of operators.
 * Used by ONLY ASFormatter.cpp
 *
 * @param operators             a reference to the vector to be built.
 */
void ASResource::buildOperators(std::vector<const std::string*>* operators, int fileType)
{
	const size_t elements = 50;
	operators->reserve(elements);

	operators->emplace_back(&AS_PLUS_ASSIGN);
	operators->emplace_back(&AS_MINUS_ASSIGN);
	operators->emplace_back(&AS_MULT_ASSIGN);
	operators->emplace_back(&AS_DIV_ASSIGN);
	operators->emplace_back(&AS_MOD_ASSIGN);
	operators->emplace_back(&AS_OR_ASSIGN);
	operators->emplace_back(&AS_AND_ASSIGN);
	operators->emplace_back(&AS_XOR_ASSIGN);
	operators->emplace_back(&AS_EQUAL);
	operators->emplace_back(&AS_PLUS_PLUS);
	operators->emplace_back(&AS_MINUS_MINUS);
	operators->emplace_back(&AS_NOT_EQUAL);
	operators->emplace_back(&AS_GR_EQUAL);
	operators->emplace_back(&AS_GR_GR_GR_ASSIGN);
	operators->emplace_back(&AS_GR_GR_ASSIGN);
	operators->emplace_back(&AS_GR_GR_GR);
	operators->emplace_back(&AS_GR_GR);
	operators->emplace_back(&AS_LS_EQUAL);
	operators->emplace_back(&AS_LS_LS_LS_ASSIGN);
	operators->emplace_back(&AS_LS_LS_ASSIGN);
	operators->emplace_back(&AS_LS_LS_LS);
	operators->emplace_back(&AS_LS_LS);
	operators->emplace_back(&AS_QUESTION_QUESTION);
	operators->emplace_back(&AS_LAMBDA);
	operators->emplace_back(&AS_ARROW);
	operators->emplace_back(&AS_AND);
	operators->emplace_back(&AS_OR);
	operators->emplace_back(&AS_SCOPE_RESOLUTION);
	operators->emplace_back(&AS_PLUS);
	operators->emplace_back(&AS_MINUS);
	operators->emplace_back(&AS_MULT);
	operators->emplace_back(&AS_DIV);
	operators->emplace_back(&AS_MOD);
	operators->emplace_back(&AS_QUESTION);
	operators->emplace_back(&AS_COLON);
	operators->emplace_back(&AS_ASSIGN);
	operators->emplace_back(&AS_LS);
	operators->emplace_back(&AS_GR);
	operators->emplace_back(&AS_NOT);
	operators->emplace_back(&AS_BIT_OR);
	operators->emplace_back(&AS_BIT_AND);
	operators->emplace_back(&AS_BIT_NOT);
	operators->emplace_back(&AS_BIT_XOR);
	if (fileType == C_TYPE)
	{
		operators->emplace_back(&AS_GCC_MIN_ASSIGN);
		operators->emplace_back(&AS_GCC_MAX_ASSIGN);
		operators->emplace_back(&AS_SPACESHIP);
	}
	if (fileType == JS_TYPE)
	{
		operators->emplace_back(&AS_EQUAL_JS);
	}
	if (fileType == SHARP_TYPE)
    {
    	operators->emplace_back(&AS_COALESCE_CS);
    }
	assert(operators->size() < elements);
	sort(operators->begin(), operators->end(), sortOnLength);
}

/**
 * Build the vector of pre-block statements.
 * Used by ONLY ASBeautifier.cpp
 * NOTE: Cannot be both a header and a preBlockStatement.
 *
 * @param preBlockStatements        a reference to the vector to be built.
 */
void ASResource::buildPreBlockStatements(std::vector<const std::string*>* preBlockStatements, int fileType)
{
	const size_t elements = 10;
	preBlockStatements->reserve(elements);

	preBlockStatements->emplace_back(&AS_CLASS);
	if (fileType == C_TYPE || fileType == OBJC_TYPE)
	{
		preBlockStatements->emplace_back(&AS_STRUCT);
		preBlockStatements->emplace_back(&AS_UNION);
		preBlockStatements->emplace_back(&AS_NAMESPACE);
		preBlockStatements->emplace_back(&AS_MODULE);     // for CORBA IDL
		preBlockStatements->emplace_back(&AS_INTERFACE);  // for CORBA IDL
	}
	if (fileType == JAVA_TYPE)
	{
		preBlockStatements->emplace_back(&AS_INTERFACE);
		preBlockStatements->emplace_back(&AS_THROWS);
	}
	if (fileType == SHARP_TYPE)
	{
		preBlockStatements->emplace_back(&AS_INTERFACE);
		preBlockStatements->emplace_back(&AS_NAMESPACE);
		preBlockStatements->emplace_back(&AS_WHERE);
		preBlockStatements->emplace_back(&AS_STRUCT);
	}

	assert(preBlockStatements->size() < elements);
	sort(preBlockStatements->begin(), preBlockStatements->end(), sortOnName);
}

/**
 * Build the vector of pre-command headers.
 * Used by BOTH ASFormatter.cpp and ASBeautifier.cpp.
 * NOTE: Cannot be both a header and a preCommandHeader.
 *
 * A preCommandHeader is in a function definition between
 * the closing paren and the opening brace.
 * e.g. in "void foo() const {}", "const" is a preCommandHeader.
 */
void ASResource::buildPreCommandHeaders(std::vector<const std::string*>* preCommandHeaders, int fileType)
{
	const size_t elements = 10;
	preCommandHeaders->reserve(elements);

	if (fileType == C_TYPE || fileType == OBJC_TYPE)
	{
		preCommandHeaders->emplace_back(&AS_CONST);
		preCommandHeaders->emplace_back(&AS_FINAL);
		preCommandHeaders->emplace_back(&AS_INTERRUPT);
		preCommandHeaders->emplace_back(&AS_NOEXCEPT);
		preCommandHeaders->emplace_back(&AS_OVERRIDE);
		preCommandHeaders->emplace_back(&AS_VOLATILE);
		preCommandHeaders->emplace_back(&AS_SEALED);			// Visual C only
	}

	if (fileType == OBJC_TYPE)
	{
		preCommandHeaders->emplace_back(&AS_AUTORELEASEPOOL);	// Obj-C only
	}

	if (fileType == JAVA_TYPE)
	{
		preCommandHeaders->emplace_back(&AS_THROWS);
	}

	if (fileType == SHARP_TYPE)
	{
		preCommandHeaders->emplace_back(&AS_WHERE);
	}

	assert(preCommandHeaders->size() < elements);
	sort(preCommandHeaders->begin(), preCommandHeaders->end(), sortOnName);
}

/**
 * Build the vector of pre-definition headers.
 * Used by ONLY ASFormatter.cpp
 * NOTE: Do NOT add 'enum' here. It is an array type brace.
 * NOTE: Do NOT add 'extern' here. Do not want an extra indent.
 *
 * @param preDefinitionHeaders      a reference to the vector to be built.
 */
void ASResource::buildPreDefinitionHeaders(std::vector<const std::string*>* preDefinitionHeaders, int fileType)
{
	const size_t elements = 10;
	preDefinitionHeaders->reserve(elements);

	preDefinitionHeaders->emplace_back(&AS_CLASS);
	if (fileType == C_TYPE || fileType == OBJC_TYPE)
	{
		preDefinitionHeaders->emplace_back(&AS_STRUCT);
		preDefinitionHeaders->emplace_back(&AS_UNION);
		preDefinitionHeaders->emplace_back(&AS_NAMESPACE);
		preDefinitionHeaders->emplace_back(&AS_MODULE);     // for CORBA IDL
		preDefinitionHeaders->emplace_back(&AS_INTERFACE);  // for CORBA IDL
	}
	if (fileType == JAVA_TYPE)
	{
		preDefinitionHeaders->emplace_back(&AS_INTERFACE);
	}
	if (fileType == SHARP_TYPE)
	{
		preDefinitionHeaders->emplace_back(&AS_STRUCT);
		preDefinitionHeaders->emplace_back(&AS_INTERFACE);
		preDefinitionHeaders->emplace_back(&AS_NAMESPACE);
	}

	assert(preDefinitionHeaders->size() < elements);
	sort(preDefinitionHeaders->begin(), preDefinitionHeaders->end(), sortOnName);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                             ASBase Functions
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// check if a specific line position contains a header.
const std::string* ASBase::findHeader(const std::string& line, int i,
                                      const std::vector<const std::string*>* possibleHeaders) const
{
	assert(isCharPotentialHeader(line, i));
	// check the word
	size_t maxHeaders = possibleHeaders->size();
	for (size_t p = 0; p < maxHeaders; p++)
	{
		const std::string* header = (*possibleHeaders)[p];
		const size_t wordEnd = i + header->length();
		if (wordEnd > line.length())
			continue;
		int result = (line.compare(i, header->length(), *header));
		if (result > 0)
			continue;
		if (result < 0)
			break;
		// check that this is not part of a longer word
		if (wordEnd == line.length())
			return header;
		if (isLegalNameChar(line[wordEnd]))
			continue;
		const char peekChar = peekNextChar(line, wordEnd - 1);
		// is not a header if part of a definition
		if (peekChar == ',' || peekChar == ')')
			break;
		// the following accessor definitions are NOT headers
		// goto default; is NOT a header
		// default(int) keyword in C# is NOT a header
		if ((header == &AS_GET
		        || header == &AS_SET
		        || header == &AS_DEFAULT)
		        && (peekChar == '.' || peekChar == ';' || peekChar == '(' || peekChar == '='))
			break;
		return header;
	}
	return nullptr;
}

// check if a specific line position contains a keyword.
bool ASBase::findKeyword(const std::string& line, int i, const std::string& keyword) const
{
	assert(isCharPotentialHeader(line, i));
	// check the word
	const size_t keywordLength = keyword.length();
	const size_t wordEnd = i + keywordLength;
	if (wordEnd > line.length())
		return false;
	if (line.compare(i, keywordLength, keyword) != 0)
		return false;
	// check that this is not part of a longer word
	if (wordEnd == line.length())
	{
		//std::cerr << "findKeyword t1 \n";
		return true;
	}

	if (isLegalNameChar(line[wordEnd]))
		return false;
	// is not a keyword if part of a definition
	const char peekChar = peekNextChar(line, (int) wordEnd - 1);
	if (peekChar == ',' || peekChar == ')')
		return false;

	//std::cerr << "findKeyword t2 \n";
	return true;
}

// check if a specific line position contains an operator.
const std::string* ASBase::findOperator(const std::string& line, int i,
                                        const std::vector<const std::string*>* possibleOperators) const
{
	assert(isCharPotentialOperator(line[i]));
	// find the operator in the vector
	// the vector contains the LONGEST operators first
	// must loop thru the entire vector
	size_t maxOperators = possibleOperators->size();
	for (size_t p = 0; p < maxOperators; p++)
	{
		const size_t wordEnd = i + (*(*possibleOperators)[p]).length();
		if (wordEnd > line.length())
			continue;
		if (line.compare(i, (*(*possibleOperators)[p]).length(), *(*possibleOperators)[p]) == 0)
			return (*possibleOperators)[p];
	}
	return nullptr;
}

// get the current word on a line
// index must point to the beginning of the word
std::string ASBase::getCurrentWord(const std::string& line, size_t index) const
{
	//assert(isCharPotentialHeader(line, index));
	size_t lineLength = line.length();
	size_t i;
	for (i = index; i < lineLength; i++)
	{
		if (!isLegalNameChar(line[i])
			|| ( (isCStyle() || isJavaStyle()) && i > index && line[i]=='.')
			)
			break;
	}
	return line.substr(index, i - index);
}

// check if a specific character can be used in a legal variable/method/class name
bool ASBase::isLegalNameChar(char ch) const
{
	if (isWhiteSpace(ch))
		return false;
	if ((unsigned char) ch > 127)
		return false;
	return (isalnum((unsigned char) ch)
            || ch == '_'
            || (!isSharpStyle() && ch == '.' )
	        || (isJavaStyle() && ch == '$')
	        || (isSharpStyle() && ch == '@'));  // may be used as a prefix
}

// check if a specific character can be part of a header
bool ASBase::isCharPotentialHeader(const std::string& line, size_t i) const
{
	assert(!isWhiteSpace(line[i]));
	char prevCh = ' ';
	if (i > 0)
		prevCh = line[i - 1];
	if (i > 1 && line[i - 2] == '\\')
		prevCh = ' ';
	if (!isLegalNameChar(prevCh) && isLegalNameChar(line[i]))
		return true;
	return false;
}

// check if a specific character can be part of an operator
bool ASBase::isCharPotentialOperator(char ch) const
{
	assert(!isWhiteSpace(ch));
	if ((unsigned) ch > 127)
		return false;
	return (ispunct((unsigned char) ch)
	        && ch != '{' && ch != '}'
	        && ch != '(' && ch != ')'
	        && ch != '[' && ch != ']'
	        && ch != ';' && ch != ','
	        && ch != '#' && ch != '\\'
	        && ch != '\'' && ch != '\"');
}

// check if a specific character is a digit
// NOTE: Visual C isdigit() gives assert error if char > 256
bool ASBase::isDigit(char ch) const
{
	return (ch >= '0' && ch <= '9');
}

// check if a specific character is a digit separator
bool ASBase::isDigitSeparator(const std::string& line, int i) const
{
	assert(line[i] == '\'');
	// casting to (unsigned char) eliminates negative characters
	// will get a "Debug Assertion Failed" if not cast
	bool foundDigitSeparator = i > 0
	                           && isxdigit((unsigned char) line[i - 1])
	                           && i < (int) line.length() - 1
	                           && isxdigit((unsigned char) line[i + 1]);
	return foundDigitSeparator;
}

// peek at the next unread character.
char ASBase::peekNextChar(const std::string& line, int i) const
{
	char ch = ' ';
	size_t peekNum = line.find_first_not_of(" \t", i + 1);
	if (peekNum == std::string::npos)
		return ch;
	ch = line[peekNum];
	return ch;
}

}   // end namespace astyle
