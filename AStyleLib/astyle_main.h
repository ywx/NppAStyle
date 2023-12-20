// astyle_main.h
// Copyright (c) 2023 The Artistic Style Authors.
// This code is licensed under the MIT License.
// License.md describes the conditions under which this software may be distributed.

#ifndef ASTYLE_MAIN_H
#define ASTYLE_MAIN_H

//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

#include "astyle.h"

#include <ctime>
#include <sstream>

#if defined(__BORLANDC__) && __BORLANDC__ < 0x0650
	// Embarcadero needs this for the following utime.h
	// otherwise "struct utimbuf" gets an error on time_t
	// 0x0650 for C++Builder XE3
	using std::time_t;
#endif

#if defined(_MSC_VER)
	#include <sys/stat.h>
	#include <sys/utime.h>
#else
	#include <sys/stat.h>
	#include <utime.h>
#endif                         // end compiler checks

#ifdef ASTYLE_JNI
	#include <jni.h>
	#ifndef ASTYLE_LIB    // ASTYLE_LIB must be defined for ASTYLE_JNI
		#define ASTYLE_LIB
	#endif
#endif  //  ASTYLE_JNI

#ifndef ASTYLE_LIB
	// for console build only
	#include "ASLocalizer.h"
	#define _(a) localizer.settext(a)
#endif	// ASTYLE_LIB

//-----------------------------------------------------------------------------
// declarations
//-----------------------------------------------------------------------------

// for getenv and localtime
#if defined(_MSC_VER)
	#pragma warning(disable: 4996)  // secure version deprecation warnings
#endif

#ifdef __clang__
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"  // getenv, localtime
	#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

#ifdef ASTYLE_LIB
	// define STDCALL and EXPORT for Windows
	// MINGW defines STDCALL in Windows.h (actually windef.h)
	// EXPORT has no value if ASTYLE_NO_EXPORT is defined
	#ifdef _WIN32
		#ifndef STDCALL
			#define STDCALL __stdcall
		#endif
		// define this to prevent compiler warning and error messages
		#ifdef ASTYLE_NO_EXPORT
			#define EXPORT
		#else
			#define EXPORT __declspec(dllexport)
		#endif
		// define STDCALL and EXPORT for non-Windows
		// visibility attribute allows "-fvisibility=hidden" compiler option
	#else
		#define STDCALL
		#if __GNUC__ >= 4
			#define EXPORT __attribute__ ((visibility ("default")))
		#else
			#define EXPORT
		#endif
	#endif	// #ifdef _WIN32
	// define pointers to callback error handler and memory allocation
	typedef void (STDCALL* fpError)(int errorNumber, const char* errorMessage);
	typedef char* (STDCALL* fpAlloc)(unsigned long memoryNeeded);
#endif  // #ifdef ASTYLE_LIB

//----------------------------------------------------------------------------
// astyle namespace
//----------------------------------------------------------------------------

namespace astyle {
//
//----------------------------------------------------------------------------
// ASStreamIterator class
// typename will be stringstream for AStyle
// it could be istream for plug-ins
// ASSourceIterator is an inherited abstract class defined in astyle.h
//----------------------------------------------------------------------------

template<typename T>
class ASStreamIterator : public ASSourceIterator
{
public:
	bool checkForEmptyLine;

public:	// function declarations
	explicit ASStreamIterator(T* in);
	~ASStreamIterator() override;
	bool getLineEndChange(int lineEndFormat) const;
	int  getStreamLength() const override;
	std::string nextLine(bool emptyLineWasDeleted) override;
	std::string peekNextLine() override;
	void peekReset() override;
	void saveLastInputLine();
	std::streamoff tellg() override;

private:
	T* inStream;            // pointer to the input stream
	std::string buffer;          // current input line
	std::string prevBuffer;      // previous input line
	std::string outputEOL;       // next output end of line char
	int eolWindows;         // number of Windows line endings, CRLF
	int eolLinux;           // number of Linux line endings, LF
	int eolMacOld;          // number of old Mac line endings. CR
	std::streamoff streamLength; // length of the input file stream
	std::streamoff peekStart;    // starting position for peekNextLine
	bool prevLineDeleted;   // the previous input line was deleted

public:	// inline functions
	bool compareToInputBuffer(const std::string& nextLine_) const
	{ return (nextLine_ == prevBuffer); }
	const std::string& getOutputEOL() const { return outputEOL; }
	std::streamoff getPeekStart() const override { return peekStart; }
	bool hasMoreLines() const override { return !inStream->eof(); }
};

//----------------------------------------------------------------------------
// ASEncoding class for utf8/16 conversions
// used by both console and library builds
//----------------------------------------------------------------------------

class ASEncoding
{
private:
	using utf16 = char16_t;       // 16 bits unsigned
	using utf8  = unsigned char;  // 8 bits
	using ubyte = unsigned char;  // 8 bits
	enum { SURROGATE_LEAD_FIRST = 0xD800 };
	enum { SURROGATE_LEAD_LAST = 0xDBFF };
	enum { SURROGATE_TRAIL_FIRST = 0xDC00 };
	enum { SURROGATE_TRAIL_LAST = 0xDFFF };
	enum { SURROGATE_FIRST_VALUE = 0x10000 };
	enum eState { eStart, eSecondOf4Bytes, ePenultimate, eFinal };

public:
	bool   getBigEndian() const;
	int    swap16bit(int value) const;
	size_t utf16len(const utf16* utf16In) const;
	size_t utf8LengthFromUtf16(const char* utf16In, size_t inLen, bool isBigEndian) const;
	size_t utf8ToUtf16(char* utf8In, size_t inLen, bool isBigEndian, char* utf16Out) const;
	size_t utf16LengthFromUtf8(const char* utf8In, size_t len) const;
	size_t utf16ToUtf8(char* utf16In, size_t inLen, bool isBigEndian,
	                   bool firstBlock, char* utf8Out) const;
};

//----------------------------------------------------------------------------
// ASOptions class for options processing
// used by both console and library builds
//----------------------------------------------------------------------------
class ASConsole;

class ASOptions
{
public:
#ifdef ASTYLE_LIB
	ASOptions(ASFormatter& formatterArg);
#else
	ASOptions(ASFormatter& formatterArg, ASConsole& consoleArg);
#endif
	std::string getOptionErrors() const;
	void importOptions(std::stringstream& in, std::vector<std::string>& optionsVector);
	bool parseOptions(std::vector<std::string>& optionsVector, const std::string& errorInfo);

private:
	// variables
	ASFormatter& formatter;
	std::stringstream optionErrors;		// option error messages
#ifndef ASTYLE_LIB
	ASConsole& console;				// DO NOT USE for ASTYLE_LIB
#endif

	// functions
	std::string getParam(const std::string& arg, const char* op);
	std::string getParam(const std::string& arg, const char* op1, const char* op2);
	bool isOption(const std::string& arg, const char* op);
	bool isOption(const std::string& arg, const char* op1, const char* op2);
	void isOptionError(const std::string& arg, const std::string& errorInfo);
	bool isParamOption(const std::string& arg, const char* option);
	bool isParamOption(const std::string& arg, const char* option1, const char* option2);
	void parseOption(const std::string& arg, const std::string& errorInfo);
	bool parseOptionContinued(const std::string& arg, const std::string& errorInfo);
};

#ifndef	ASTYLE_LIB

//----------------------------------------------------------------------------
// ASConsole class for console build
//----------------------------------------------------------------------------

class ASConsole
{
private:    // variables
	ASFormatter& formatter;             // reference to the ASFormatter object
	ASEncoding encode;                  // file encoding conversion
	ASLocalizer localizer;              // language translation
	std::ostream* errorStream;               // direct error messages to cerr or std::cout
	// command line options
	bool isRecursive;                   // recursive option
	bool isDryRun;                      // dry-run option
	bool noBackup;                      // suffix=none option
	bool preserveDate;                  // preserve-date option
	bool isVerbose;                     // verbose option
	bool isQuiet;                       // quiet option
	bool isFormattedOnly;               // formatted lines only option
	bool ignoreExcludeErrors;           // don't abort on unmatched excludes
	bool ignoreExcludeErrorsDisplay;    // don't display unmatched excludes
	bool useAscii;                      // ascii option
	// other variables
	bool bypassBrowserOpen;             // don't open the browser on html options
	bool hasWildcard;                   // file name includes a wildcard
	size_t mainDirectoryLength;         // directory length to be excluded in displays
	bool filesAreIdentical;             // input and output files are identical
	int  filesFormatted;                // number of files formatted
	int  filesUnchanged;                // number of files unchanged
	bool lineEndsMixed;                 // output has mixed line ends
	int  linesOut;                      // number of output lines

	std::string outputEOL;                   // current line end
	std::string prevEOL;                     // previous line end
	std::string astyleExePath;               // absolute executable path and name from argv[0]
	std::string optionFileName;              // file path and name of the options file
	std::string origSuffix;                  // suffix= option
	std::string projectOptionFileName;       // file path and name of the project options file
	std::string stdPathIn;                   // path to input from stdin=
	std::string stdPathOut;                  // path to output from stdout=
	std::string targetDirectory;             // path to the directory being processed
	std::string targetFilename;              // file name being processed

	std::vector<std::string> excludeVector;       // exclude from wildcard hits
	std::vector<bool>   excludeHitsVector;   // exclude flags for error reporting
	std::vector<std::string> fileNameVector;      // file paths and names from the command line
	std::vector<std::string> optionsVector;       // options from the command line
	std::vector<std::string> projectOptionsVector;// project options from the project options file
	std::vector<std::string> fileOptionsVector;   // options from the options file
	std::vector<std::string> fileName;            // files to be processed including path

public:     // functions
	explicit ASConsole(ASFormatter& formatterArg);
	ASConsole(const ASConsole&)            = delete;
	ASConsole& operator=(ASConsole const&) = delete;
	void convertLineEnds(std::ostringstream& out, int lineEnd);
	FileEncoding detectEncoding(const char* data, size_t dataSize) const;
	void error() const;
	void error(const char* why, const char* what) const;
	void formatCinToCout();
	std::vector<std::string> getArgvOptions(int argc, char** argv);
	bool fileExists(const char* file) const;
	bool fileNameVectorIsEmpty() const;
	std::ostream* getErrorStream() const;
	bool getFilesAreIdentical() const;
	int  getFilesFormatted() const;
	bool getIgnoreExcludeErrors() const;
	bool getIgnoreExcludeErrorsDisplay() const;
	bool getIsDryRun() const;
	bool getIsFormattedOnly() const;
	bool getIsQuiet() const;
	bool getIsRecursive() const;
	bool getIsVerbose() const;
	bool getLineEndsMixed() const;
	bool getNoBackup() const;
	bool getPreserveDate() const;
	std::string getLanguageID() const;
	std::string getNumberFormat(int num, size_t lcid = 0) const;
	std::string getNumberFormat(int num, const char* groupingArg, const char* separator) const;
	std::string getOptionFileName() const;
	std::string getOrigSuffix() const;
	std::string getProjectOptionFileName() const;
	std::string getStdPathIn() const;
	std::string getStdPathOut() const;
	void getTargetFilenames(std::string& targetFilename_, std::vector<std::string>& targetFilenameVector) const;
	void processFiles();
	void processOptions(const std::vector<std::string>& argvOptions);
	void setBypassBrowserOpen(bool state);
	void setErrorStream(std::ostream* errStreamPtr);
	void setIgnoreExcludeErrors(bool state);
	void setIgnoreExcludeErrorsAndDisplay(bool state);
	void setIsDryRun(bool state);
	void setIsFormattedOnly(bool state);
	void setIsQuiet(bool state);
	void setIsRecursive(bool state);
	void setIsVerbose(bool state);
	void setNoBackup(bool state);
	void setOptionFileName(const std::string& name);
	void setOrigSuffix(const std::string& suffix);
	void setPreserveDate(bool state);
	void setProjectOptionFileName(const std::string& optfilepath);
	void setStdPathIn(const std::string& path);
	void setStdPathOut(const std::string& path);
	void standardizePath(std::string& path, bool removeBeginningSeparator = false) const;
	bool stringEndsWith(const std::string& str, const std::string& suffix) const;
	void updateExcludeVector(const std::string& suffixParam);
	std::vector<std::string> getExcludeVector() const;
	std::vector<bool>   getExcludeHitsVector() const;
	std::vector<std::string> getFileNameVector() const;
	std::vector<std::string> getOptionsVector() const;
	std::vector<std::string> getProjectOptionsVector() const;
	std::vector<std::string> getFileOptionsVector() const;
	std::vector<std::string> getFileName() const;

private:	// functions
	void correctMixedLineEnds(std::ostringstream& out);
	void formatFile(const std::string& fileName_);
	std::string getParentDirectory(const std::string& absPath) const;
	std::string findProjectOptionFilePath(const std::string& fileName_) const;
	std::string getCurrentDirectory(const std::string& fileName_) const;
	void getFileNames(const std::string& directory, const std::vector<std::string>& wildcards);
	void getFilePaths(const std::string& filePath);
	std::string getFullPathName(const std::string& relativePath) const;
	std::string getHtmlInstallPrefix() const;
	std::string getParam(const std::string& arg, const char* op);
	bool isHomeOrInvalidAbsPath(const std::string& absPath) const;
	void initializeOutputEOL(LineEndFormat lineEndFormat);
	bool isOption(const std::string& arg, const char* op);
	bool isOption(const std::string& arg, const char* a, const char* b);
	bool isParamOption(const std::string& arg, const char* option);
	bool isPathExclued(const std::string& subPath);
	void launchDefaultBrowser(const char* filePathIn = nullptr) const;
	void printHelp() const;
	void printMsg(const char* msg, const std::string& data) const;
	void printSeparatingLine() const;
	void printVerboseHeader() const;
	void printVerboseStats(clock_t startTime) const;
	FileEncoding readFile(const std::string& fileName_, std::stringstream& in) const;
	void removeFile(const char* fileName_, const char* errMsg) const;
	void renameFile(const char* oldFileName, const char* newFileName, const char* errMsg) const;
	void setOutputEOL(LineEndFormat lineEndFormat, const std::string& currentEOL);
	void sleep(int seconds) const;
	int  waitForRemove(const char* newFileName) const;
	int  wildcmp(const char* wild, const char* data) const;
	void writeFile(const std::string& fileName_, FileEncoding encoding, std::ostringstream& out) const;
#ifdef _WIN32
	void displayLastError();
#endif
};
#else	// ASTYLE_LIB

//----------------------------------------------------------------------------
// ASLibrary class for library build
//----------------------------------------------------------------------------

class ASLibrary
{
public:
	ASLibrary()          = default;
	virtual ~ASLibrary() = default;
	// virtual functions are mocked in testing
	char16_t* formatUtf16(const char16_t*, const char16_t*, fpError, fpAlloc) const;
	virtual char16_t* convertUtf8ToUtf16(const char* utf8In, fpAlloc fpMemoryAlloc) const;
	virtual char* convertUtf16ToUtf8(const char16_t* utf16In) const;

private:
	static char* STDCALL tempMemoryAllocation(unsigned long memoryNeeded);

private:
	ASEncoding encode;             // file encoding conversion
};

#endif	// ASTYLE_LIB

//----------------------------------------------------------------------------

}   // end of namespace astyle

//----------------------------------------------------------------------------
// declarations for java native interface (JNI) build
// they are called externally and are NOT part of the namespace
//----------------------------------------------------------------------------
#ifdef ASTYLE_JNI
void  STDCALL javaErrorHandler(int errorNumber, const char* errorMessage);
char* STDCALL javaMemoryAlloc(unsigned long memoryNeeded);
// the following function names are constructed from method names in the calling java program
extern "C" EXPORT
jstring STDCALL Java_AStyleInterface_AStyleGetVersion(JNIEnv* env, jclass);
extern "C" EXPORT
jstring STDCALL Java_AStyleInterface_AStyleMain(JNIEnv* env,
                                                jobject obj,
                                                jstring textInJava,
                                                jstring optionsJava);
#endif //  ASTYLE_JNI

//----------------------------------------------------------------------------
// declarations for UTF-16 interface
// they are called externally and are NOT part of the namespace
//----------------------------------------------------------------------------
#ifdef ASTYLE_LIB
extern "C" EXPORT
char16_t* STDCALL AStyleMainUtf16(const char16_t* pSourceIn,
                                  const char16_t* pOptions,
                                  fpError fpErrorHandler,
                                  fpAlloc fpMemoryAlloc);
#endif	// ASTYLE_LIB

//-----------------------------------------------------------------------------
// declarations for standard DLL interface
// they are called externally and are NOT part of the namespace
//-----------------------------------------------------------------------------
#ifdef ASTYLE_LIB
extern "C" EXPORT char* STDCALL AStyleMain(const char* pSourceIn,
                                           const char* pOptions,
                                           fpError fpErrorHandler,
                                           fpAlloc fpMemoryAlloc);
extern "C" EXPORT const char* STDCALL AStyleGetVersion(void);
#endif	// ASTYLE_LIB

//-----------------------------------------------------------------------------

#endif // closes ASTYLE_MAIN_H
