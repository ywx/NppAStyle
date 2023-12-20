// astyle_main.cpp
// Copyright (c) 2023 The Artistic Style Authors.
// This code is licensed under the MIT License.
// License.md describes the conditions under which this software may be distributed.

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   AStyle_main source file map.
 *   This source file contains several classes.
 *   They are arranged as follows.
 *   ---------------------------------------
 *   namespace astyle {
 *   ASStreamIterator methods
 *   ASConsole methods
 *      // Windows specific
 *      // Linux specific
 *   ASLibrary methods
 *      // Windows specific
 *      // Linux specific
 *   ASOptions methods
 *   ASEncoding methods
 *   }  // end of astyle namespace
 *   Global Area ---------------------------
 *      Java Native Interface functions
 *      AStyleMainUtf16 entry point
 *      AStyleMain entry point
 *      AStyleGetVersion entry point
 *      main entry point
 *  ---------------------------------------
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//-----------------------------------------------------------------------------
// headers
//-----------------------------------------------------------------------------

#include "astyle_main.h"

#include <algorithm>
#include <cerrno>
#include <clocale>		// needed by some compilers
#include <cstdlib>
#include <fstream>
#include <sstream>

// includes for recursive getFileNames() function
#ifdef _WIN32
	#undef UNICODE		// use ASCII windows functions
	#include <Windows.h>
#else
	#include <dirent.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#ifdef __VMS
		#include <unixlib.h>
		#include <rms.h>
		#include <ssdef.h>
		#include <stsdef.h>
		#include <lib$routines.h>
		#include <starlet.h>
	#endif /* __VMS */
#endif

//-----------------------------------------------------------------------------
// declarations
//-----------------------------------------------------------------------------

// turn off MinGW automatic file globbing
// this CANNOT be in the astyle namespace
#ifndef ASTYLE_LIB
	int _CRT_glob = 0;
#endif

//----------------------------------------------------------------------------
// astyle namespace
//----------------------------------------------------------------------------

namespace astyle {
//
// console build variables
#ifndef ASTYLE_LIB
	#ifdef _WIN32
		char g_fileSeparator = '\\';     // Windows file separator
		bool g_isCaseSensitive = false;  // Windows IS NOT case sensitive
	#else
		char g_fileSeparator = '/';      // Linux file separator
		bool g_isCaseSensitive = true;   // Linux IS case sensitive
	#endif	// _WIN32
#endif	// ASTYLE_LIB

// java library build variables
#ifdef ASTYLE_JNI
	JNIEnv*   g_env;
	jobject   g_obj;
	jmethodID g_mid;
#endif

const char* g_version = ASTYLE_VERSION;

//-----------------------------------------------------------------------------
// ASStreamIterator class
// typename will be stringstream for AStyle
// it could be istream or wxChar for plug-ins
//-----------------------------------------------------------------------------

template<typename T>
ASStreamIterator<T>::ASStreamIterator(T* in)
{
	inStream = in;
	buffer.reserve(200);
	eolWindows = 0;
	eolLinux = 0;
	eolMacOld = 0;
	peekStart = 0;
	prevLineDeleted = false;
	checkForEmptyLine = false;
	// get length of stream
	inStream->seekg(0, inStream->end);
	streamLength = inStream->tellg();
	inStream->seekg(0, inStream->beg);
}

template<typename T>
ASStreamIterator<T>::~ASStreamIterator() = default;

/**
* get the length of the input stream.
* streamLength variable is set by the constructor.
*
* @return     length of the input file stream, converted to an int.
*/
template<typename T>
int ASStreamIterator<T>::getStreamLength() const
{
	return static_cast<int>(streamLength);
}

/**
 * read the input stream, delete any end of line characters,
 *     and build a string that contains the input line.
 *
 * @return        string containing the next input line minus any end of line characters
 */
template<typename T>
std::string ASStreamIterator<T>::nextLine(bool emptyLineWasDeleted)
{
	// verify that the current position is correct
	assert(peekStart == 0);

	// a deleted line may be replaced if break-blocks is requested
	// this sets up the compare to check for a replaced empty line
	if (prevLineDeleted)
	{
		prevLineDeleted = false;
		checkForEmptyLine = true;
	}
	if (!emptyLineWasDeleted)
		prevBuffer = buffer;
	else
		prevLineDeleted = true;

	// read the next record
	buffer.clear();
	char ch;
	inStream->get(ch);

	while (!inStream->eof() && ch != '\n' && ch != '\r')
	{
		buffer.append(1, ch);
		inStream->get(ch);
	}

	if (inStream->eof())
	{
		return buffer;
	}

	int peekCh = inStream->peek();

	// find input end-of-line characters
	if (!inStream->eof())
	{
		if (ch == '\r')         // CR+LF is windows otherwise Mac OS 9
		{
			if (peekCh == '\n')
			{
				inStream->get();
				eolWindows++;
			}
			else
				eolMacOld++;
		}
		else                    // LF is Linux, allow for improbable LF/CR
		{
			if (peekCh == '\r')
			{
				inStream->get();
				eolWindows++;
			}
			else
				eolLinux++;
		}
	}
	else
	{
		inStream->clear();
	}

	// has not detected an input end of line
	if (!eolWindows && !eolLinux && !eolMacOld)
	{
#ifdef _WIN32
		eolWindows++;
#else
		eolLinux++;
#endif
	}

	// set output end of line characters
	if (eolWindows >= eolLinux)
	{
		if (eolWindows >= eolMacOld)
			outputEOL = "\r\n";     // Windows (CR+LF)
		else
			outputEOL = "\r";       // MacOld (CR)
	}
	else if (eolLinux >= eolMacOld)
		outputEOL = "\n";           // Linux (LF)
	else
		outputEOL = "\r";           // MacOld (CR)

	return buffer;
}

// save the current position and get the next line
// this can be called for multiple reads
// when finished peeking you MUST call peekReset()
// call this function from ASFormatter ONLY
template<typename T>
std::string ASStreamIterator<T>::peekNextLine()
{
	assert(hasMoreLines());
	std::string nextLine_;
	char ch;

	if (!peekStart)
		peekStart = inStream->tellg();

	// read the next record
	inStream->get(ch);
	while (!inStream->eof() && ch != '\n' && ch != '\r')
	{
		nextLine_.append(1, ch);
		inStream->get(ch);
	}

	if (inStream->eof())
	{
		return nextLine_;
	}

	int peekCh = inStream->peek();

	// remove end-of-line characters
	if (!inStream->eof())
	{
		if ((peekCh == '\n' || peekCh == '\r') && peekCh != ch)
			inStream->get();
	}

	return nextLine_;
}

// reset current position and EOF for peekNextLine()
template<typename T>
void ASStreamIterator<T>::peekReset()
{
	assert(peekStart != 0);
	inStream->clear();
	inStream->seekg(peekStart);
	peekStart = 0;
}

// save the last input line after input has reached EOF
template<typename T>
void ASStreamIterator<T>::saveLastInputLine()
{
	assert(inStream->eof());
	prevBuffer = buffer;
}

// return position of the get pointer
template<typename T>
std::streamoff ASStreamIterator<T>::tellg()
{
	return inStream->tellg();
}

// check for a change in line ends
template<typename T>
bool ASStreamIterator<T>::getLineEndChange(int lineEndFormat) const
{
	assert(lineEndFormat == LINEEND_DEFAULT
	       || lineEndFormat == LINEEND_WINDOWS
	       || lineEndFormat == LINEEND_LINUX
	       || lineEndFormat == LINEEND_MACOLD);

	bool lineEndChange = false;
	if (lineEndFormat == LINEEND_WINDOWS)
		lineEndChange = (eolLinux + eolMacOld != 0);
	else if (lineEndFormat == LINEEND_LINUX)
		lineEndChange = (eolWindows + eolMacOld != 0);
	else if (lineEndFormat == LINEEND_MACOLD)
		lineEndChange = (eolWindows + eolLinux != 0);
	else
	{
		if (eolWindows > 0)
			lineEndChange = (eolLinux + eolMacOld != 0);
		else if (eolLinux > 0)
			lineEndChange = (eolWindows + eolMacOld != 0);
		else if (eolMacOld > 0)
			lineEndChange = (eolWindows + eolLinux != 0);
	}
	return lineEndChange;
}

//-----------------------------------------------------------------------------
// ASConsole class
// main function will be included only in the console build
//-----------------------------------------------------------------------------

#ifndef ASTYLE_LIB

ASConsole::ASConsole(ASFormatter& formatterArg) : formatter(formatterArg)
{
	errorStream = &std::cerr;
	// command line options
	isRecursive = false;
	isDryRun = false;
	noBackup = false;
	preserveDate = false;
	isVerbose = false;
	isQuiet = false;
	isFormattedOnly = false;
	ignoreExcludeErrors = false;
	ignoreExcludeErrorsDisplay = false;
	useAscii = false;
	// other variables
	bypassBrowserOpen = false;
	hasWildcard = false;
	filesAreIdentical = true;
	lineEndsMixed = false;
	origSuffix = ".orig";
	mainDirectoryLength = 0;
	filesFormatted = 0;
	filesUnchanged = 0;
	linesOut = 0;
}

// rewrite a stringstream converting the line ends
void ASConsole::convertLineEnds(std::ostringstream& out, int lineEnd)
{
	assert(lineEnd == LINEEND_WINDOWS || lineEnd == LINEEND_LINUX || lineEnd == LINEEND_MACOLD);
	const std::string& inStr = out.str();	// avoids strange looking syntax
	std::string outStr;						// the converted output
	int inLength = (int) inStr.length();
	for (int pos = 0; pos < inLength; pos++)
	{
		if (inStr[pos] == '\r')
		{
			if (inStr[pos + 1] == '\n')
			{
				// CRLF
				if (lineEnd == LINEEND_CR)
				{
					outStr += inStr[pos];		// Delete the LF
					pos++;
					continue;
				}
				if (lineEnd == LINEEND_LF)
				{
					outStr += inStr[pos + 1];	// Delete the CR
					pos++;
					continue;
				}
				outStr += inStr[pos];			// Do not change
				outStr += inStr[pos + 1];
				pos++;
				continue;
			}
			else                                // NOLINT
			{
				// CR
				if (lineEnd == LINEEND_CRLF)
				{
					outStr += inStr[pos];		// Insert the CR
					outStr += '\n';				// Insert the LF
					continue;
				}
				if (lineEnd == LINEEND_LF)
				{
					outStr += '\n';				// Insert the LF
					continue;
				}
				outStr += inStr[pos];		// Do not change
				continue;
			}
		}
		else if (inStr[pos] == '\n')
		{
			// LF
			if (lineEnd == LINEEND_CRLF)
			{
				outStr += '\r';				// Insert the CR
				outStr += inStr[pos];		// Insert the LF
				continue;
			}
			if (lineEnd == LINEEND_CR)
			{
				outStr += '\r';				// Insert the CR
				continue;
			}
			outStr += inStr[pos];		// Do not change
			continue;
		}
		else
		{
			outStr += inStr[pos];		// Write the current char
		}
	}
	// replace the stream
	out.str(outStr);
}

void ASConsole::correctMixedLineEnds(std::ostringstream& out)
{
	LineEndFormat lineEndFormat = LINEEND_DEFAULT;
	if (outputEOL == "\r\n")
		lineEndFormat = LINEEND_WINDOWS;
	if (outputEOL == "\n")
		lineEndFormat = LINEEND_LINUX;
	if (outputEOL == "\r")
		lineEndFormat = LINEEND_MACOLD;
	convertLineEnds(out, lineEndFormat);
}

// check files for 16 or 32 bit encoding
// the file must have a Byte Order Mark (BOM)
// NOTE: some string functions don't work with NULLs (e.g. length())
FileEncoding ASConsole::detectEncoding(const char* data, size_t dataSize) const
{
	if (dataSize >= 3 && memcmp(data, "\xEF\xBB\xBF", 3) == 0)
	{
		return UTF_8BOM;
	}

	if (dataSize >= 4)
	{
		if (memcmp(data, "\x00\x00\xFE\xFF", 4) == 0)
		{
			return UTF_32BE;
		}
		else if (memcmp(data, "\xFF\xFE\x00\x00", 4) == 0)
		{
			return UTF_32LE;
		}
	}

	if (dataSize >= 2)
	{
		if (memcmp(data, "\xFE\xFF", 2) == 0)
		{
			return UTF_16BE;
		}
		else if (memcmp(data, "\xFF\xFE", 2) == 0)
		{
			return UTF_16LE;
		}
	}
	return ENCODING_8BIT;
}

// error exit without a message
void ASConsole::error() const
{
	(*errorStream) << _("Artistic Style has terminated\n") << std::endl;
	exit(EXIT_FAILURE);
}

// error exit with a message
void ASConsole::error(const char* why, const char* what) const
{
	(*errorStream) << why << ' ' << what << std::endl;
	error();
}

/**
 * If no files have been given, use cin for input and cout for output.
 *
 * This is used to format text for text editors.
 * Do NOT display any console messages when this function is used.
 */
void ASConsole::formatCinToCout()
{
	// check for files from --stdin= and --stdout=
	if (!stdPathIn.empty())
	{
		if (!freopen(stdPathIn.c_str(), "r", stdin))
			error("Cannot open input file", stdPathIn.c_str());
	}
	if (!stdPathOut.empty())
	{
		if (!freopen(stdPathOut.c_str(), "w", stdout))
			error("Cannot open output file", stdPathOut.c_str());

	}
	// Using cin.tellg() causes problems with both Windows and Linux.
	// The Windows problem occurs when the input is not Windows line-ends.
	// The tellg() will be out of sequence with the get() statements.
	// The Linux cin.tellg() will return -1 (invalid).
	// Copying the input sequentially to a stringstream before
	// formatting solves the problem for both.
	std::istream* inStream = &std::cin;
	std::stringstream outStream;
	char ch;
	inStream->get(ch);
	while (!inStream->eof() && !inStream->fail())
	{
		outStream.put(ch);
		inStream->get(ch);
	}
	ASStreamIterator<std::stringstream> streamIterator(&outStream);
	// Windows pipe or redirection always outputs Windows line-ends.
	// Linux pipe or redirection will output any line end.
#ifdef _WIN32
	LineEndFormat lineEndFormat = LINEEND_DEFAULT;
#else
	LineEndFormat lineEndFormat = formatter.getLineEndFormat();
#endif // _WIN32
	initializeOutputEOL(lineEndFormat);
	formatter.init(&streamIterator);

	while (formatter.hasMoreLines())
	{
		std::cout << formatter.nextLine();
		if (formatter.hasMoreLines())
		{
			setOutputEOL(lineEndFormat, streamIterator.getOutputEOL());
			std::cout << outputEOL;
		}
		else
		{
			// this can happen if the file if missing a closing brace and break-blocks is requested
			if (formatter.getIsLineReady())
			{
				setOutputEOL(lineEndFormat, streamIterator.getOutputEOL());
				std::cout << outputEOL;
				std::cout << formatter.nextLine();
			}
		}
	}
	std::cout.flush();
}

/**
 * Open input file, format it, and close the output.
 *
 * @param fileName_     The path and name of the file to be processed.
 */
void ASConsole::formatFile(const std::string& fileName_)
{
	std::stringstream in;
	std::ostringstream out;
	FileEncoding encoding = readFile(fileName_, in);

	// Unless a specific language mode has been set, set the language mode
	// according to the file's suffix.
	if (!formatter.getModeManuallySet())
	{
		if (stringEndsWith(fileName_, std::string(".java")))
			formatter.setJavaStyle();
		else if (stringEndsWith(fileName_, std::string(".cs")))
			formatter.setSharpStyle();
		else
			formatter.setCStyle();
	}

	// set line end format
	std::string nextLine;				// next output line
	filesAreIdentical = true;		// input and output files are identical
	LineEndFormat lineEndFormat = formatter.getLineEndFormat();
	initializeOutputEOL(lineEndFormat);
	// do this AFTER setting the file mode
	ASStreamIterator<std::stringstream> streamIterator(&in);
	formatter.init(&streamIterator);

	// format the file
	while (formatter.hasMoreLines())
	{
		nextLine = formatter.nextLine();
		out << nextLine;
		linesOut++;
		if (formatter.hasMoreLines())
		{
			setOutputEOL(lineEndFormat, streamIterator.getOutputEOL());
			out << outputEOL;
		}
		else
		{
			streamIterator.saveLastInputLine();     // to compare the last input line
			// this can happen if the file if missing a closing brace and break-blocks is requested
			if (formatter.getIsLineReady())
			{
				setOutputEOL(lineEndFormat, streamIterator.getOutputEOL());
				out << outputEOL;
				nextLine = formatter.nextLine();
				out << nextLine;
				linesOut++;
				streamIterator.saveLastInputLine();
			}
		}

		if (filesAreIdentical)
		{
			if (streamIterator.checkForEmptyLine)
			{
				if (nextLine.find_first_not_of(" \t") != std::string::npos)
					filesAreIdentical = false;
			}
			else if (!streamIterator.compareToInputBuffer(nextLine))
				filesAreIdentical = false;
			streamIterator.checkForEmptyLine = false;
		}
	}
	// correct for mixed line ends
	if (lineEndsMixed)
	{
		correctMixedLineEnds(out);
		filesAreIdentical = false;
	}

	// remove targetDirectory from filename if required by print
	std::string displayName;
	if (hasWildcard)
		displayName = fileName_.substr(targetDirectory.length() + 1);
	else
		displayName = fileName_;

	// if file has changed, write the new file
	if (!filesAreIdentical || streamIterator.getLineEndChange(lineEndFormat))
	{
		if (!isDryRun)
			writeFile(fileName_, encoding, out);
		printMsg(_("Formatted  %s\n"), displayName);
		filesFormatted++;
	}
	else
	{
		if (!isFormattedOnly)
			printMsg(_("Unchanged  %s\n"), displayName);
		filesUnchanged++;
	}

	assert(formatter.getChecksumDiff() == 0);
}

/**
 * Searches for a file named fileName_ in the current directory. If it is not
 * found, recursively searches for fileName_ in the current directory's parent
 * directories, returning the location of the first instance of fileName_
 * found. If fileName_ is not found, an empty string is returned.
 *
 * @param fileName_     The filename the function should attempt to locate.
 * @return              The full path to fileName_ in the current directory or
 *                      nearest parent directory if found, otherwise an empty
 *                      string.
 */
std::string ASConsole::findProjectOptionFilePath(const std::string& fileName_) const
{
	std::string parent;

	if (!fileNameVector.empty())
	{
		std::string first = fileNameVector.front();

		if (first.find_first_of("*?") != std::string::npos)
		{
			// First item has wildcards - get rid of them for now
			size_t endPath = first.find_last_of(g_fileSeparator);

			if (endPath != std::string::npos)
			{
				first.erase(endPath + 1, std::string::npos);
			}
			else
			{
				first = ".";
				first.push_back(g_fileSeparator);
			}
		}

		parent = getFullPathName(first);
		if (parent[parent.size()] != g_fileSeparator)
		{
			parent.push_back(g_fileSeparator);
		}

	}
	else if (!stdPathIn.empty())
	{
		parent = getFullPathName(stdPathIn);
	}
	else
	{
		parent = getFullPathName(getCurrentDirectory(fileName_));
		if (parent.size())
		{
			parent.push_back(g_fileSeparator);
		}
	}
	// remove filename from path
	size_t endPath = parent.find_last_of(g_fileSeparator);
	if (endPath != std::string::npos)
		parent = parent.substr(0, endPath + 1);

	while (!parent.empty())
	{
		std::string filepath = parent + fileName_;
		if (fileExists(filepath.c_str()))
			return filepath;
		if (fileName_ == ".astylerc")
		{
			filepath = parent + "_astylerc";
			if (fileExists(filepath.c_str()))
				return filepath;
		}
		parent = getParentDirectory(parent);
	}
	return std::string();
}

// for unit testing
std::vector<bool> ASConsole::getExcludeHitsVector() const
{ return excludeHitsVector; }

// for unit testing
std::vector<std::string> ASConsole::getExcludeVector() const
{ return excludeVector; }

// for unit testing
std::vector<std::string> ASConsole::getFileName() const
{ return fileName; }

// for unit testing
std::vector<std::string> ASConsole::getFileNameVector() const
{ return fileNameVector; }

// for unit testing
std::vector<std::string> ASConsole::getFileOptionsVector() const
{ return fileOptionsVector; }

// for unit testing
bool ASConsole::getFilesAreIdentical() const
{ return filesAreIdentical; }

// for unit testing
int ASConsole::getFilesFormatted() const
{ return filesFormatted; }

// for unit testing
bool ASConsole::getIgnoreExcludeErrors() const
{ return ignoreExcludeErrors; }

// for unit testing
bool ASConsole::getIgnoreExcludeErrorsDisplay() const
{ return ignoreExcludeErrorsDisplay; }

// for unit testing
bool ASConsole::getIsDryRun() const
{ return isDryRun; }

// for unit testing
bool ASConsole::getIsFormattedOnly() const
{ return isFormattedOnly; }

// for unit testing
std::string ASConsole::getLanguageID() const
{ return localizer.getLanguageID(); }

// for unit testing
bool ASConsole::getIsQuiet() const
{ return isQuiet; }

// for unit testing
bool ASConsole::getIsRecursive() const
{ return isRecursive; }

// for unit testing
bool ASConsole::getIsVerbose() const
{ return isVerbose; }

// for unit testing
bool ASConsole::getLineEndsMixed() const
{ return lineEndsMixed; }

// for unit testing
bool ASConsole::getNoBackup() const
{ return noBackup; }

// for unit testing
std::string ASConsole::getOptionFileName() const
{ return optionFileName; }

// for unit testing
std::vector<std::string> ASConsole::getOptionsVector() const
{ return optionsVector; }

// for unit testing
std::string ASConsole::getOrigSuffix() const
{ return origSuffix; }

// for unit testing
bool ASConsole::getPreserveDate() const
{ return preserveDate; }

// for unit testing
std::string ASConsole::getProjectOptionFileName() const
{
	assert(projectOptionFileName.length() > 0);
	// remove the directory path
	size_t start = projectOptionFileName.find_last_of(g_fileSeparator);
	if (start == std::string::npos)
		start = 0;
	return projectOptionFileName.substr(start + 1);
}

// for unit testing
std::vector<std::string> ASConsole::getProjectOptionsVector() const
{ return projectOptionsVector; }

// for unit testing
std::string ASConsole::getStdPathIn() const
{ return stdPathIn; }

// for unit testing
std::string ASConsole::getStdPathOut() const
{ return stdPathOut; }

// for unit testing
void ASConsole::setBypassBrowserOpen(bool state)
{ bypassBrowserOpen = state; }

// for unit testing
std::ostream* ASConsole::getErrorStream() const
{
	return errorStream;
}

void ASConsole::setErrorStream(std::ostream* errStreamPtr)
{
	errorStream = errStreamPtr;
}

// build a vector of argv options
// the program path argv[0] is excluded
std::vector<std::string> ASConsole::getArgvOptions(int argc, char** argv)
{
	if (argc > 0)
		astyleExePath = getFullPathName(argv[0]);
	std::vector<std::string> argvOptions;
	for (int i = 1; i < argc; i++)
	{
		argvOptions.emplace_back(std::string(argv[i]));
	}
	return argvOptions;
}

std::string ASConsole::getParam(const std::string& arg, const char* op)
{
	return arg.substr(strlen(op));
}

void ASConsole::getTargetFilenames(std::string& targetFilename_,
                                   std::vector<std::string>& targetFilenameVector) const
{
	size_t beg = 0;
	size_t sep = 0;
	while (beg < targetFilename_.length())
	{
		// find next target
		sep = targetFilename_.find_first_of(",;", beg);
		if (sep == std::string::npos)
			sep = targetFilename_.length();
		std::string fileExtension = targetFilename_.substr(beg, sep - beg);
		beg = sep + 1;
		// remove whitespace
		while (fileExtension.length() > 0
		        && (fileExtension[0] == ' ' || fileExtension[0] == '\t'))
			fileExtension = fileExtension.erase(0, 1);
		while (fileExtension.length() > 0
		        && (fileExtension[fileExtension.length() - 1] == ' '
		            || fileExtension[fileExtension.length() - 1] == '\t'))
			fileExtension = fileExtension.erase(fileExtension.length() - 1, 1);
		if (fileExtension.length() > 0)
			targetFilenameVector.emplace_back(fileExtension);
	}
	if (targetFilenameVector.empty())
	{
		fprintf(stderr, _("Missing filename in %s\n"), targetFilename_.c_str());
		error();
	}
}

// initialize output end of line
void ASConsole::initializeOutputEOL(LineEndFormat lineEndFormat)
{
	assert(lineEndFormat == LINEEND_DEFAULT
	       || lineEndFormat == LINEEND_WINDOWS
	       || lineEndFormat == LINEEND_LINUX
	       || lineEndFormat == LINEEND_MACOLD);

	outputEOL.clear();			// current line end
	prevEOL.clear();			// previous line end
	lineEndsMixed = false;		// output has mixed line ends, LINEEND_DEFAULT only

	if (lineEndFormat == LINEEND_WINDOWS)
		outputEOL = "\r\n";
	else if (lineEndFormat == LINEEND_LINUX)
		outputEOL = "\n";
	else if (lineEndFormat == LINEEND_MACOLD)
		outputEOL = "\r";
	else
		outputEOL.clear();
}

// read a file into the stringstream 'in'
FileEncoding ASConsole::readFile(const std::string& fileName_, std::stringstream& in) const
{
	const int blockSize = 65536;	// 64 KB
	std::ifstream fin(fileName_.c_str(), std::ios::binary);
	if (!fin)
		error("Cannot open file", fileName_.c_str());
	char* data = new (std::nothrow) char[blockSize];
	if (data == nullptr)
		error("Cannot allocate memory to open file", fileName_.c_str());
	fin.read(data, blockSize);
	if (fin.bad())
		error("Cannot read file", fileName_.c_str());
	size_t dataSize = static_cast<size_t>(fin.gcount());
	FileEncoding encoding = detectEncoding(data, dataSize);
	if (encoding == UTF_32BE || encoding == UTF_32LE)
		error(_("Cannot process UTF-32 encoding"), fileName_.c_str());
	bool firstBlock = true;
	bool isBigEndian = (encoding == UTF_16BE);

	while (dataSize != 0)
	{
		if (encoding == UTF_16LE || encoding == UTF_16BE)
		{
			// convert utf-16 to utf-8
			size_t utf8Size = encode.utf8LengthFromUtf16(data, dataSize, isBigEndian);
			char* utf8Out = new (std::nothrow) char[utf8Size];
			if (utf8Out == nullptr)
				error("Cannot allocate memory for utf-8 conversion", fileName_.c_str());
			size_t utf8Len = encode.utf16ToUtf8(data, dataSize, isBigEndian, firstBlock, utf8Out);
			assert(utf8Len <= utf8Size);
			in << std::string(utf8Out, utf8Len);
			delete[] utf8Out;
		}
		else
			in << std::string(data, dataSize);
		fin.read(data, blockSize);
		if (fin.bad())
			error("Cannot read file", fileName_.c_str());
		dataSize = static_cast<size_t>(fin.gcount());
		firstBlock = false;
	}
	fin.close();
	delete[] data;
	return encoding;
}

void ASConsole::setIgnoreExcludeErrors(bool state)
{ ignoreExcludeErrors = state; }

void ASConsole::setIgnoreExcludeErrorsAndDisplay(bool state)
{ ignoreExcludeErrors = state; ignoreExcludeErrorsDisplay = state; }

void ASConsole::setIsFormattedOnly(bool state)
{ isFormattedOnly = state; }

void ASConsole::setIsQuiet(bool state)
{ isQuiet = state; }

void ASConsole::setIsRecursive(bool state)
{ isRecursive = state; }

void ASConsole::setIsDryRun(bool state)
{ isDryRun = state; }

void ASConsole::setIsVerbose(bool state)
{ isVerbose = state; }

void ASConsole::setNoBackup(bool state)
{ noBackup = state; }

void ASConsole::setOptionFileName(const std::string& name)
{ optionFileName = name; }

void ASConsole::setOrigSuffix(const std::string& suffix)
{ origSuffix = suffix; }

void ASConsole::setPreserveDate(bool state)
{ preserveDate = state; }

void ASConsole::setProjectOptionFileName(const std::string& optfilepath)
{ projectOptionFileName = optfilepath; }

void ASConsole::setStdPathIn(const std::string& path)
{ stdPathIn = path; }

void ASConsole::setStdPathOut(const std::string& path)
{ stdPathOut = path; }

// set outputEOL variable
void ASConsole::setOutputEOL(LineEndFormat lineEndFormat, const std::string& currentEOL)
{
	if (lineEndFormat == LINEEND_DEFAULT)
	{
		outputEOL = currentEOL;
		if (prevEOL.empty())
			prevEOL = outputEOL;
		if (prevEOL != outputEOL)
		{
			lineEndsMixed = true;
			filesAreIdentical = false;
			prevEOL = outputEOL;
		}
	}
	else
	{
		prevEOL = currentEOL;
		if (prevEOL != outputEOL)
			filesAreIdentical = false;
	}
}

#ifdef _WIN32  // Windows specific

/**
 * WINDOWS function to display the last system error.
 */
void ASConsole::displayLastError()
{
	LPSTR msgBuf;
	DWORD lastError = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	              nullptr,
	              lastError,
	              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // Default language
	              (LPSTR) &msgBuf,
	              0,
	              nullptr
	             );
	// Display the string.
	(*errorStream) << "Error (" << lastError << ") " << msgBuf << std::endl;
	// Free the buffer.
	LocalFree(msgBuf);
}

/**
 * WINDOWS function to get the current directory.
 * NOTE: getenv("CD") does not work for Windows Vista.
 *        The Windows function GetCurrentDirectory is used instead.
 *
 * @return              The path of the current directory
 */
std::string ASConsole::getCurrentDirectory(const std::string& fileName_) const
{
	char currdir[MAX_PATH];
	currdir[0] = '\0';
	if (!GetCurrentDirectory(sizeof(currdir), currdir))
		error("Cannot find file", fileName_.c_str());
	return std::string(currdir);
}

/**
 * WINDOWS function to resolve wildcards and recurse into sub directories.
 * The fileName vector is filled with the path and names of files to process.
 *
 * @param directory     The path of the directory to be processed.
 * @param wildcards     A vector of wildcards to be processed (e.g. *.cpp).
 */
void ASConsole::getFileNames(const std::string& directory, const std::vector<std::string>& wildcards)
{
	std::vector<std::string> subDirectory;    // sub directories of directory
	WIN32_FIND_DATA findFileData;   // for FindFirstFile and FindNextFile

	// Find the first file in the directory
	// Find will get at least "." and "..".
	std::string firstFile = directory + "\\*";
	HANDLE hFind = FindFirstFile(firstFile.c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		// Error (3) The system cannot find the path specified.
		// Error (123) The filename, directory name, or volume label syntax is incorrect.
		// ::FindClose(hFind); before exiting
		displayLastError();
		error(_("Cannot open directory"), directory.c_str());
	}

	// save files and sub directories
	do
	{
		// skip hidden or read only
		if (findFileData.cFileName[0] == '.'
		        || (findFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
		        || (findFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
			continue;

		// is this a sub directory
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!isRecursive)
				continue;
			// if a sub directory and recursive, save sub directory
			std::string subDirectoryPath = directory + g_fileSeparator + findFileData.cFileName;
			if (isPathExclued(subDirectoryPath))
				printMsg(_("Exclude  %s\n"), subDirectoryPath.substr(mainDirectoryLength));
			else
				subDirectory.emplace_back(subDirectoryPath);
			continue;
		}

		std::string filePathName = directory + g_fileSeparator + findFileData.cFileName;
		// check exclude before wildcmp to avoid "unmatched exclude" error
		bool isExcluded = isPathExclued(filePathName);
		// save file name if wildcard match
		for (const std::string& wildcard : wildcards)
		{
			if (wildcmp(wildcard.c_str(), findFileData.cFileName))
			{
				if (isExcluded)
					printMsg(_("Exclude  %s\n"), filePathName.substr(mainDirectoryLength));
				else
					fileName.emplace_back(filePathName);
				break;
			}
		}
	}
	while (FindNextFile(hFind, &findFileData) != 0);

	// check for processing error
	::FindClose(hFind);
	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
		error("Error processing directory", directory.c_str());

	// recurse into sub directories
	// if not doing recursive subDirectory is empty
	for (const std::string& subDirectoryName : subDirectory)
		getFileNames(subDirectoryName, wildcards);
}

// WINDOWS function to get the full path name from the relative path name
// Return the full path name or an empty string if failed.
std::string ASConsole::getFullPathName(const std::string& relativePath) const
{
	char fullPath[MAX_PATH];
	GetFullPathName(relativePath.c_str(), MAX_PATH, fullPath, nullptr);
	return fullPath;
}

/**
 * WINDOWS function to format a number according to the current locale.
 * This formats positive integers only, no float.
 *
 * @param num		The number to be formatted.
 * @param lcid		The LCID of the locale to be used for testing.
 * @return			The formatted number.
 */
std::string ASConsole::getNumberFormat(int num, size_t lcid) const
{
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__GNUC__)
	// Compilers that don't support C++ locales should still support this assert.
	// The C locale should be set but not the C++.
	// This function is not necessary if the C++ locale is set.
	// The locale().name() return value is not portable to all compilers.
	assert(std::locale().name() == "C");
#endif
	// convert num to a string
	std::stringstream alphaNum;
	alphaNum << num;
	std::string number = alphaNum.str();
	if (useAscii)
		return number;

	// format the number using the Windows API
	if (lcid == 0)
		lcid = LOCALE_USER_DEFAULT;
	int outSize = ::GetNumberFormat(lcid, 0, number.c_str(), nullptr, nullptr, 0);
	char* outBuf = new (std::nothrow) char[outSize];
	if (outBuf == nullptr)
		return number;
	::GetNumberFormat(lcid, 0, number.c_str(), nullptr, outBuf, outSize);
	std::string formattedNum(outBuf);
	delete[] outBuf;
	// remove the decimal
	int decSize = ::GetLocaleInfo(lcid, LOCALE_SDECIMAL, nullptr, 0);
	char* decBuf = new (std::nothrow) char[decSize];
	if (decBuf == nullptr)
		return number;
	::GetLocaleInfo(lcid, LOCALE_SDECIMAL, decBuf, decSize);
	size_t i = formattedNum.rfind(decBuf);
	delete[] decBuf;
	if (i != std::string::npos)
		formattedNum.erase(i);
	if (!formattedNum.length())
		formattedNum = "0";
	return formattedNum;
}

/**
 * WINDOWS function to check for a HOME directory
 *
 * @param absPath       The path to be evaluated.
 * @returns             true if absPath is HOME or is an invalid absolute
 *                      path, false otherwise.
 */
bool ASConsole::isHomeOrInvalidAbsPath(const std::string& absPath) const
{
	const char* const env = getenv("USERPROFILE");
	if (env == nullptr)
		return true;

	return (absPath.compare(env) == 0);
}

/**
 * WINDOWS function to open a HTML file in the default browser.
 */
void ASConsole::launchDefaultBrowser(const char* filePathIn /*nullptr*/) const
{
	struct stat statbuf;
	const char* envPaths[] = { "PROGRAMFILES(X86)", "PROGRAMFILES" };
	size_t pathsLen = sizeof(envPaths) / sizeof(envPaths[0]);
	std::string htmlDefaultPath;
	for (size_t i = 0; i < pathsLen; i++)
	{
		const char* const envPath = getenv(envPaths[i]);
		if (envPath == nullptr)
			continue;
		htmlDefaultPath = envPath;
		if (htmlDefaultPath.length() > 0
		        && htmlDefaultPath[htmlDefaultPath.length() - 1] == g_fileSeparator)
			htmlDefaultPath.erase(htmlDefaultPath.length() - 1);
		htmlDefaultPath.append("\\AStyle\\doc");
		if (stat(htmlDefaultPath.c_str(), &statbuf) == 0 && statbuf.st_mode & S_IFDIR)
			break;
	}
	htmlDefaultPath.append("\\");

	// build file path
	std::string htmlFilePath;
	if (filePathIn == nullptr)
		htmlFilePath = htmlDefaultPath + "astyle.html";
	else
	{
		if (strpbrk(filePathIn, "\\/") == nullptr)
			htmlFilePath = htmlDefaultPath + filePathIn;
		else
			htmlFilePath = filePathIn;
	}
	standardizePath(htmlFilePath);
	if (stat(htmlFilePath.c_str(), &statbuf) != 0 || !(statbuf.st_mode & S_IFREG))
	{
		printf(_("Cannot open HTML file %s\n"), htmlFilePath.c_str());
		return;
	}

	SHELLEXECUTEINFO sei = { sizeof(sei), {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} };
	sei.fMask = SEE_MASK_FLAG_NO_UI;
	sei.lpVerb = "open";
	sei.lpFile = htmlFilePath.c_str();
	sei.nShow = SW_SHOWNORMAL;

	// browser open will be bypassed in test programs
	printf(_("Opening HTML documentation %s\n"), htmlFilePath.c_str());
	if (!bypassBrowserOpen)
	{
		int ret = ShellExecuteEx(&sei);
		if (!ret)
			error(_("Command execute failure"), htmlFilePath.c_str());
	}
}

#else  // Linux specific

/**
 * LINUX function to get the current directory.
 * This is done if the fileName does not contain a path.
 * It is probably from an editor sending a single file.
 *
 * @param fileName_     The filename is used only for  the error message.
 * @return              The path of the current directory
 */
std::string ASConsole::getCurrentDirectory(const std::string& fileName_) const
{
	const char* const currdir = getenv("PWD");
	if (currdir == nullptr)
		error("Cannot find file", fileName_.c_str());
	return std::string(currdir);
}

/**
 * LINUX function to resolve wildcards and recurse into sub directories.
 * The fileName vector is filled with the path and names of files to process.
 *
 * @param directory     The path of the directory to be processed.
 * @param wildcards     A vector of wildcards to be processed (e.g. *.cpp).
 */
void ASConsole::getFileNames(const std::string& directory, const std::vector<std::string>& wildcards)
{
	struct dirent* entry;           // entry from readdir()
	struct stat statbuf;            // entry from stat()
	std::vector<std::string> subDirectory;    // sub directories of this directory

	// errno is defined in <errno.h> and is set for errors in opendir, readdir, or stat
	errno = 0;

	DIR* dp = opendir(directory.c_str());
	if (dp == nullptr)
		error(_("Cannot open directory"), directory.c_str());

	// save the first fileName entry for this recursion
	const unsigned firstEntry = fileName.size();

	// save files and sub directories
	while ((entry = readdir(dp)) != nullptr)
	{
		// get file status
		std::string entryFilepath = directory + g_fileSeparator + entry->d_name;
		if (stat(entryFilepath.c_str(), &statbuf) != 0)
		{
			if (errno == EOVERFLOW)         // file over 2 GB is OK
			{
				errno = 0;
				continue;
			}
			perror("errno message");
			//error("Error getting file status in directory", directory.c_str());
			error("Error getting file status for", entryFilepath.c_str());
		}
		// skip hidden or read only
		if (entry->d_name[0] == '.' || !(statbuf.st_mode & S_IWUSR))
			continue;
		// if a sub directory and recursive, save sub directory
		if (S_ISDIR(statbuf.st_mode) && isRecursive)
		{
			if (isPathExclued(entryFilepath))
				printMsg(_("Exclude  %s\n"), entryFilepath.substr(mainDirectoryLength));
			else
				subDirectory.emplace_back(entryFilepath);
			continue;
		}

		// if a file, save file name
		if (S_ISREG(statbuf.st_mode))
		{
			// check exclude before wildcmp to avoid "unmatched exclude" error
			bool isExcluded = isPathExclued(entryFilepath);
			// save file name if wildcard match
			for (const std::string& wildcard : wildcards)
			{
				if (wildcmp(wildcard.c_str(), entry->d_name) != 0)
				{
					if (isExcluded)
						printMsg(_("Exclude  %s\n"), entryFilepath.substr(mainDirectoryLength));
					else
						fileName.emplace_back(entryFilepath);
					break;
				}
			}
		}
	}

	if (closedir(dp) != 0)
	{
		perror("errno message");
		error("Error reading directory", directory.c_str());
	}

	// sort the current entries for fileName
	if (firstEntry < fileName.size())
		sort(fileName.begin() + firstEntry, fileName.end());

	// recurse into sub directories
	// if not doing recursive, subDirectory is empty
	if (subDirectory.size() > 1)
		sort(subDirectory.begin(), subDirectory.end());
	for (unsigned i = 0; i < subDirectory.size(); i++)
	{
		getFileNames(subDirectory[i], wildcards);
	}
}

// LINUX function to get the full path name from the relative path name
// Return the full path name or an empty std::string if failed.
std::string ASConsole::getFullPathName(const std::string& relativePath) const
{
	char* fullPath = realpath(relativePath.c_str(), nullptr);
	if (fullPath == nullptr)
		return std::string();
	const std::string p(fullPath);
	free(fullPath);
	return p;
}

// LINUX function to get the documentation file path prefix
//     from the executable file path.
// Return the documentation path prefix or an empty std::string if failed.
std::string ASConsole::getHtmlInstallPrefix() const
{
	std::string astyleHtmlPrefix = astyleExePath;
	size_t end = astyleHtmlPrefix.find("/bin/");
	if (end == std::string::npos)
		return "";
	astyleHtmlPrefix = astyleHtmlPrefix.substr(0, end);
	return astyleHtmlPrefix;
}

/**
 * LINUX function to get locale information and call getNumberFormat.
 * This formats positive integers only, no float.
 *
 * @param num		The number to be formatted.
 *                  size_t is for compatibility with the Windows function.
 * @return			The formatted number.
 */
std::string ASConsole::getNumberFormat(int num, size_t /*lcid*/) const
{
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__BORLANDC__) || defined(__GNUC__)
	// Compilers that don't support C++ locales should still support this assert.
	// The C locale should be set but not the C++.
	// This function is not necessary if the C++ locale is set.
	// The locale().name() return value is not portable to all compilers.
	assert(std::locale().name() == "C");
#endif

	// get the locale info
	struct lconv* lc;
	lc = localeconv();

	// format the number
	return getNumberFormat(num, lc->grouping, lc->thousands_sep);
}

/**
 * LINUX function to format a number according to the current locale.
 * This formats positive integers only, no float.
 *
 * @param num			The number to be formatted.
 * @param groupingArg   The grouping string from the locale.
 * @param  separator	The thousands group separator from the locale.
 * @return				The formatted number.
 */
std::string ASConsole::getNumberFormat(int num, const char* groupingArg, const char* separator) const
{
	// convert num to a string
	std::stringstream alphaNum;
	alphaNum << num;
	std::string number = alphaNum.str();
	// format the number from right to left
	std::string formattedNum;
	size_t ig = 0;	// grouping index
	int grouping = groupingArg[ig];
	int i = number.length();
	// check for no grouping
	if (grouping == 0)
		grouping = number.length();
	while (i > 0)
	{
		// extract a group of numbers
		std::string group;
		if (i < grouping)
			group = number;
		else
			group = number.substr(i - grouping);
		// update formatted number
		formattedNum.insert(0, group);
		i -= grouping;
		if (i < 0)
			i = 0;
		if (i > 0)
			formattedNum.insert(0, separator);
		number.erase(i);
		// update grouping
		if (groupingArg[ig] != '\0'
		        && groupingArg[ig + 1] != '\0')
			grouping = groupingArg[++ig];
	}
	return formattedNum;
}

/**
 * LINUX function to check for a HOME directory
 *
 * @param absPath       The path to be evaluated.
 * @returns             true if absPath is HOME or is an invalid absolute
 *                      path, false otherwise.
 */
bool ASConsole::isHomeOrInvalidAbsPath(const std::string& absPath) const
{
	const char* const env = getenv("HOME");

	if (env == nullptr)
		return true;

	return (absPath.compare(env) == 0 || absPath == "/" );
}

/**
 * LINUX function to open a HTML file in the default browser.
 * Use xdg-open from freedesktop.org cross-desktop compatibility suite xdg-utils.
 * see http://portland.freedesktop.org/wiki/
 * This is installed on most modern distributions.
 */
void ASConsole::launchDefaultBrowser(const char* filePathIn /*nullptr*/) const
{
#ifdef __APPLE__
	std::string htmlDefaultPrefix = "/usr/local";
#else
	std::string htmlDefaultPrefix = "/usr";
#endif
	std::string htmlDefaultPath = htmlDefaultPrefix + "/share/doc/astyle/html/";
	std::string htmlDefaultFile = "astyle.html";
	std::string htmlFilePath;
	struct stat statbuf;

	// build html path
	if (filePathIn == nullptr)
	{
		std::string htmlPrefix = getHtmlInstallPrefix();
		if (htmlPrefix.empty())
			htmlFilePath = htmlDefaultPrefix + htmlDefaultPath + htmlDefaultFile;
		else
			htmlFilePath = htmlPrefix + htmlDefaultPath + htmlDefaultFile;
	}
	else
	{
		if (strpbrk(filePathIn, "\\/") == nullptr)
			htmlFilePath = htmlDefaultPath + filePathIn;
		else
			htmlFilePath = filePathIn;
	}
	standardizePath(htmlFilePath);
	if (stat(htmlFilePath.c_str(), &statbuf) != 0 || !(statbuf.st_mode & S_IFREG))
	{
		printf(_("Cannot open HTML file %s\n"), htmlFilePath.c_str());
		return;
	}

	// get search paths
	const char* const envPaths = getenv("PATH");
	if (envPaths == nullptr)
		error("Cannot read PATH environment variable", "");
	size_t envlen = strlen(envPaths);
	char* paths = new char[envlen + 1];
	strcpy(paths, envPaths);
	// find xdg-open (usually in /usr/bin)
	// Mac uses open instead
#ifdef __APPLE__
	const char* fileOpen = "open";
#else
	const char* fileOpen = "xdg-open";
#endif
	std::string searchPath;
	char* searchDir = strtok(paths, ":");
	while (searchDir != nullptr)
	{
		searchPath = searchDir;
		if (searchPath.length() > 0
		        && searchPath[searchPath.length() - 1] != g_fileSeparator)
			searchPath.append(std::string(1, g_fileSeparator));
		searchPath.append(fileOpen);
		if (stat(searchPath.c_str(), &statbuf) == 0 && (statbuf.st_mode & S_IFREG))
			break;
		searchDir = strtok(nullptr, ":");
	}
	delete[] paths;
	if (searchDir == nullptr)
		error(_("Command is not installed"), fileOpen);

	// browser open will be bypassed in test programs
	printf(_("Opening HTML documentation %s\n"), htmlFilePath.c_str());
	if (!bypassBrowserOpen)
	{
		execlp(fileOpen, fileOpen, htmlFilePath.c_str(), nullptr);
		// execlp will NOT return if successful
		error(_("Command execute failure"), fileOpen);
	}
}

#endif  // _WIN32

/**
 * Returns the parent directory of absPath. If absPath is not a valid absolute
 * path or if it does not have a parent, an empty string is returned.
 *
 * @param absPath       The initial directory.
 * @return              The parent directory of absPath, or an empty string if
 *                      one cannot be found.
 */
std::string ASConsole::getParentDirectory(const std::string& absPath) const
{
	if (isHomeOrInvalidAbsPath(absPath))
	{
		return std::string();
	}
	size_t offset = absPath.size() - 1;
	if (absPath[absPath.size() - 1] == g_fileSeparator)
	{
		offset -= 1;
	}
	size_t idx = absPath.rfind(g_fileSeparator, offset);
	if (idx == std::string::npos)
	{
		return std::string();
	}
	std::string str = absPath.substr(0, idx + 1);
	return str;
}

// get individual file names from the command-line file path
void ASConsole::getFilePaths(const std::string& filePath)
{
	fileName.clear();
	targetDirectory = std::string();
	targetFilename = std::string();
	std::vector<std::string> targetFilenameVector;

	// separate directory and file name
	size_t separator = filePath.find_last_of(g_fileSeparator);
	if (separator == std::string::npos)
	{
		// if no directory is present, use the currently active directory
		targetDirectory = getCurrentDirectory(filePath);
		targetFilename  = filePath;
		mainDirectoryLength = targetDirectory.length() + 1;    // +1 includes trailing separator
	}
	else
	{
		targetDirectory = filePath.substr(0, separator);
		targetFilename  = filePath.substr(separator + 1);
		mainDirectoryLength = targetDirectory.length() + 1;    // +1 includes trailing separator
	}

	if (targetFilename.length() == 0)
	{
		fprintf(stderr, _("Missing filename in %s\n"), filePath.c_str());
		error();
	}

	// check filename for wildcards
	hasWildcard = false;
	if (targetFilename.find_first_of("*?") != std::string::npos)
		hasWildcard = true;

	// If the filename is not quoted on Linux, bash will replace the
	// wildcard instead of passing it to the program.
	if (isRecursive && !hasWildcard)
	{
		fprintf(stderr, "%s\n", _("Recursive option with no wildcard"));
#ifndef _WIN32
		fprintf(stderr, "%s\n", _("Did you intend quote the filename"));
#endif
		error();
	}

	bool hasMultipleTargets = false;
	if (targetFilename.find_first_of(",;") != std::string::npos)
		hasMultipleTargets = true;

	// display directory name for wildcard processing
	if (hasWildcard)
	{
		printSeparatingLine();
		printMsg(_("Directory  %s\n"), targetDirectory + g_fileSeparator + targetFilename);
	}

	// clear exclude hits vector
	size_t excludeHitsVectorSize = excludeHitsVector.size();
	for (size_t ix = 0; ix < excludeHitsVectorSize; ix++)
		excludeHitsVector[ix] = false;

	// create a std::vector of paths and file names to process
	if (hasWildcard || isRecursive || hasMultipleTargets)
	{
		getTargetFilenames(targetFilename, targetFilenameVector);
		getFileNames(targetDirectory, targetFilenameVector);
	}
	else
	{
		// verify a single file is not a directory (needed on Linux)
		std::string entryFilepath = targetDirectory + g_fileSeparator + targetFilename;
		struct stat statbuf;
		if (stat(entryFilepath.c_str(), &statbuf) == 0 && (statbuf.st_mode & S_IFREG))
			fileName.emplace_back(entryFilepath);
	}

	// check for unprocessed excludes
	bool excludeErr = false;
	for (size_t ix = 0; ix < excludeHitsVector.size(); ix++)
	{
		if (!excludeHitsVector[ix])
		{
			excludeErr = true;
			if (!ignoreExcludeErrorsDisplay)
			{
				if (ignoreExcludeErrors)
					printMsg(_("Exclude (unmatched)  %s\n"), excludeVector[ix]);
				else
					fprintf(stderr, _("Exclude (unmatched)  %s\n"), excludeVector[ix].c_str());
			}
			else
			{
				if (!ignoreExcludeErrors)
					fprintf(stderr, _("Exclude (unmatched)  %s\n"), excludeVector[ix].c_str());
			}
		}
	}

	if (excludeErr && !ignoreExcludeErrors)
	{
		if (hasWildcard && !isRecursive)
			fprintf(stderr, "%s\n", _("Did you intend to use --recursive"));
		error();
	}

	// check if files were found (probably an input error if not)
	if (fileName.empty())
	{
		fprintf(stderr, _("No file to process %s\n"), filePath.c_str());
		if (hasWildcard && !isRecursive)
			fprintf(stderr, "%s\n", _("Did you intend to use --recursive"));
		error();
	}

	if (hasWildcard)
		printSeparatingLine();
}

// Check if a file exists
bool ASConsole::fileExists(const char* file) const
{
	struct stat buf;
	return (stat(file, &buf) == 0);
}

bool ASConsole::fileNameVectorIsEmpty() const
{
	return fileNameVector.empty();
}

bool ASConsole::isOption(const std::string& arg, const char* op)
{
	return arg == op;
}

bool ASConsole::isOption(const std::string& arg, const char* a, const char* b)
{
	return (isOption(arg, a) || isOption(arg, b));
}

bool ASConsole::isParamOption(const std::string& arg, const char* option)
{
	bool retVal = arg.compare(0, strlen(option), option) == 0;
	// if comparing for short option, 2nd char of arg must be numeric
	if (retVal && strlen(option) == 1 && arg.length() > 1)
		if (!isdigit((unsigned char) arg[1]))
			retVal = false;
	return retVal;
}

// compare a path to the exclude std::vector
// used for both directories and filenames
// updates the g_excludeHitsVector
// return true if a match
bool ASConsole::isPathExclued(const std::string& subPath)
{
	bool retVal = false;

	// read the exclude std::vector checking for a match
	for (size_t i = 0; i < excludeVector.size(); i++)
	{
		std::string exclude = excludeVector[i];

		if (subPath.length() < exclude.length())
			continue;

		size_t compareStart = subPath.length() - exclude.length();
		// subPath compare must start with a directory name
		if (compareStart > 0)
		{
			char lastPathChar = subPath[compareStart - 1];
			if (lastPathChar != g_fileSeparator)
				continue;
		}

		std::string compare = subPath.substr(compareStart);
		if (!g_isCaseSensitive)
		{
			// make it case insensitive for Windows
			for (size_t j = 0; j < compare.length(); j++)
				compare[j] = (char) tolower(compare[j]);
			for (size_t j = 0; j < exclude.length(); j++)
				exclude[j] = (char) tolower(exclude[j]);
		}
		// compare sub directory to exclude data - must check them all
		if (compare == exclude)
		{
			excludeHitsVector[i] = true;
			retVal = true;
			break;
		}
	}
	return retVal;
}

void ASConsole::printHelp() const
{
	std::cout << std::endl;
	std::cout << "                     Artistic Style " << g_version << std::endl;
	std::cout << "                     Maintained by: Andre Simon, Jim Pattee\n";
	std::cout << "                     Original Author: Tal Davidson\n";
	std::cout << std::endl;
	std::cout << "Usage:\n";
	std::cout << "------\n";
	std::cout << "            astyle [OPTIONS] File1 File2 File3 [...]\n";
	std::cout << std::endl;
	std::cout << "            astyle [OPTIONS] < Original > Beautified\n";
	std::cout << std::endl;
	std::cout << "    When indenting a specific file, the resulting indented file RETAINS\n";
	std::cout << "    the original file-name. The original pre-indented file is renamed,\n";
	std::cout << "    with a suffix of \'.orig\' added to the original filename.\n";
	std::cout << std::endl;
	std::cout << "    Wildcards (* and ?) may be used in the filename.\n";
	std::cout << "    A \'recursive\' option can process directories recursively.\n";
	std::cout << "    Multiple file extensions may be separated by a comma.\n";
	std::cout << std::endl;
	std::cout << "    By default, astyle is set up to indent with four spaces per indent,\n";
	std::cout << "    a maximal indentation of 40 spaces inside continuous statements,\n";
	std::cout << "    a minimum indentation of eight spaces inside conditional statements,\n";
	std::cout << "    and NO formatting options.\n";
	std::cout << std::endl;
	std::cout << "Options:\n";
	std::cout << "--------\n";
	std::cout << "    This  program  follows  the  usual  GNU  command line syntax.\n";
	std::cout << "    Long options (starting with '--') must be written one at a time.\n";
	std::cout << "    Short options (starting with '-') may be appended together.\n";
	std::cout << "    Thus, -bps4 is the same as -b -p -s4.\n";
	std::cout << std::endl;
	std::cout << "Option Files:\n";
	std::cout << "-------------\n";
	std::cout << "    Artistic Style looks for a default option file and/or a project\n";
	std::cout << "    option file in the following order:\n";
	std::cout << "    1. The command line options have precedence.\n";
	std::cout << "    2. The project option file has precedence over the default file\n";
	std::cout << "       o the file name indicated by the --project= command line option.\n";
	std::cout << "       o the file named .astylerc or _ astylerc.\n";
	std::cout << "       o the file name identified by ARTISTIC_STYLE_PROJECT_OPTIONS.\n";
	std::cout << "       o the file is disabled by --project=none on the command line.\n";
	std::cout << "    3. The default option file that can be used for all projects.\n";
	std::cout << "       o the file path indicated by the --options= command line option.\n";
	std::cout << "       o the file path indicated by ARTISTIC_STYLE_OPTIONS.\n";
	std::cout << "       o the file named .astylerc in the HOME directory (for Linux).\n";
	std::cout << "       o the file name astylerc in the APPDATA directory (for Windows).\n";
	std::cout << "       o the file is disabled by --project=none on the command line.\n";
	std::cout << "    Long options within the option files may be written without '--'.\n";
	std::cout << "    Line-end comments begin with a '#'.\n";
	std::cout << std::endl;
	std::cout << "Disable Formatting:\n";
	std::cout << "-------------------\n";
	std::cout << "    Disable Block\n";
	std::cout << "    Blocks of code can be disabled with the comment tags *INDENT-OFF*\n";
	std::cout << "    and *INDENT-ON*. It must be contained in a one-line comment.\n";
	std::cout << std::endl;
	std::cout << "    Disable Line\n";
	std::cout << "    Padding of operators can be disabled on a single line using the\n";
	std::cout << "    comment tag *NOPAD*. It must be contained in a line-end comment.\n";
	std::cout << std::endl;
	std::cout << "Brace Style Options:\n";
	std::cout << "--------------------\n";
	std::cout << "    default brace style\n";
	std::cout << "    If no brace style is requested, the opening braces will not be\n";
	std::cout << "    changed and closing braces will be broken from the preceding line.\n";
	std::cout << std::endl;
	std::cout << "    --style=allman  OR  --style=bsd  OR  --style=break  OR  -A1\n";
	std::cout << "    Allman style formatting/indenting.\n";
	std::cout << "    Broken braces.\n";
	std::cout << std::endl;
	std::cout << "    --style=java  OR  --style=attach  OR  -A2\n";
	std::cout << "    Java style formatting/indenting.\n";
	std::cout << "    Attached braces.\n";
	std::cout << std::endl;
	std::cout << "    --style=kr  OR  --style=k&r  OR  --style=k/r  OR  -A3\n";
	std::cout << "    Kernighan & Ritchie style formatting/indenting.\n";
	std::cout << "    Linux braces.\n";
	std::cout << std::endl;
	std::cout << "    --style=stroustrup  OR  -A4\n";
	std::cout << "    Stroustrup style formatting/indenting.\n";
	std::cout << "    Linux braces, with broken closing headers.\n";
	std::cout << std::endl;
	std::cout << "    --style=whitesmith  OR  -A5\n";
	std::cout << "    Whitesmith style formatting/indenting.\n";
	std::cout << "    Broken, indented braces.\n";
	std::cout << "    Indented class blocks and switch blocks.\n";
	std::cout << std::endl;
	std::cout << "    --style=vtk  OR  -A15\n";
	std::cout << "    VTK style formatting/indenting.\n";
	std::cout << "    Broken, indented braces except for the opening braces.\n";
	std::cout << std::endl;
	std::cout << "    --style=ratliff  OR  --style=banner  OR  -A6\n";
	std::cout << "    Ratliff style formatting/indenting.\n";
	std::cout << "    Attached, indented braces.\n";
	std::cout << std::endl;
	std::cout << "    --style=gnu  OR  -A7\n";
	std::cout << "    GNU style formatting/indenting.\n";
	std::cout << "    Broken braces, indented blocks.\n";
	std::cout << std::endl;
	std::cout << "    --style=linux  OR  --style=knf  OR  -A8\n";
	std::cout << "    Linux style formatting/indenting.\n";
	std::cout << "    Linux braces, minimum conditional indent is one-half indent.\n";
	std::cout << std::endl;
	std::cout << "    --style=horstmann  OR  --style=run-in  OR  -A9\n";
	std::cout << "    Horstmann style formatting/indenting.\n";
	std::cout << "    Run-in braces, indented switches.\n";
	std::cout << std::endl;
	std::cout << "    --style=1tbs  OR  --style=otbs  OR  -A10\n";
	std::cout << "    One True Brace Style formatting/indenting.\n";
	std::cout << "    Linux braces, add braces to all conditionals.\n";
	std::cout << std::endl;
	std::cout << "    --style=google  OR  -A14\n";
	std::cout << "    Google style formatting/indenting.\n";
	std::cout << "    Attached braces, indented class modifiers.\n";
	std::cout << std::endl;
	std::cout << "    --style=mozilla  OR  -A16\n";
	std::cout << "    Mozilla style formatting/indenting.\n";
	std::cout << "    Linux braces, with broken braces for structs and enums,\n";
	std::cout << "    and attached braces for namespaces.\n";
	std::cout << std::endl;
	std::cout << "    --style=webkit  OR  -A17\n";
	std::cout << "    WebKit style formatting/indenting.\n";
	std::cout << "    Linux braces, with attached closing headers.\n";
	std::cout << std::endl;
	std::cout << "    --style=pico  OR  -A11\n";
	std::cout << "    Pico style formatting/indenting.\n";
	std::cout << "    Run-in opening braces and attached closing braces.\n";
	std::cout << "    Uses keep one line blocks and keep one line statements.\n";
	std::cout << std::endl;
	std::cout << "    --style=lisp  OR  -A12\n";
	std::cout << "    Lisp style formatting/indenting.\n";
	std::cout << "    Attached opening braces and attached closing braces.\n";
	std::cout << "    Uses keep one line statements.\n";
	std::cout << std::endl;
	std::cout << "Tab Options:\n";
	std::cout << "------------\n";
	std::cout << "    default indent option\n";
	std::cout << "    If no indentation option is set, the default\n";
	std::cout << "    option of 4 spaces per indent will be used.\n";
	std::cout << std::endl;
	std::cout << "    --indent=spaces=#  OR  -s#\n";
	std::cout << "    Indent using # spaces per indent. Not specifying #\n";
	std::cout << "    will result in a default of 4 spaces per indent.\n";
	std::cout << std::endl;
	std::cout << "    --indent=tab  OR  --indent=tab=#  OR  -t  OR  -t#\n";
	std::cout << "    Indent using tab characters, assuming that each\n";
	std::cout << "    indent is # spaces long. Not specifying # will result\n";
	std::cout << "    in a default assumption of 4 spaces per indent.\n";
	std::cout << std::endl;
	std::cout << "    --indent=force-tab=#  OR  -T#\n";
	std::cout << "    Indent using tab characters, assuming that each\n";
	std::cout << "    indent is # spaces long. Force tabs to be used in areas\n";
	std::cout << "    AStyle would prefer to use spaces.\n";
	std::cout << std::endl;
	std::cout << "    --indent=force-tab-x=#  OR  -xT#\n";
	std::cout << "    Allows the tab length to be set to a length that is different\n";
	std::cout << "    from the indent length. This may cause the indentation to be\n";
	std::cout << "    a mix of both spaces and tabs. This option sets the tab length.\n";
	std::cout << std::endl;
	std::cout << "Brace Modify Options:\n";
	std::cout << "---------------------\n";
	std::cout << "    --attach-namespaces  OR  -xn\n";
	std::cout << "    Attach braces to a namespace statement.\n";
	std::cout << std::endl;
	std::cout << "    --attach-classes  OR  -xc\n";
	std::cout << "    Attach braces to a class statement.\n";
	std::cout << std::endl;
	std::cout << "    --attach-inlines  OR  -xl\n";
	std::cout << "    Attach braces to class inline function definitions.\n";
	std::cout << std::endl;
	std::cout << "    --attach-extern-c  OR  -xk\n";
	std::cout << "    Attach braces to an extern \"C\" statement.\n";
	std::cout << std::endl;
	std::cout << "    --attach-closing-while  OR  -xV\n";
	std::cout << "    Attach closing while of do-while to the closing brace.\n";
	std::cout << std::endl;
	std::cout << "Indentation Options:\n";
	std::cout << "--------------------\n";
	std::cout << "    --indent-classes  OR  -C\n";
	std::cout << "    Indent 'class' blocks so that the entire block is indented.\n";
	std::cout << std::endl;
	std::cout << "    --indent-modifiers  OR  -xG\n";
	std::cout << "    Indent 'class' access modifiers, 'public:', 'protected:' or\n";
	std::cout << "    'private:', one half indent. The rest of the class is not\n";
	std::cout << "    indented. \n";
	std::cout << std::endl;
	std::cout << "    --indent-switches  OR  -S\n";
	std::cout << "    Indent 'switch' blocks, so that the inner 'case XXX:'\n";
	std::cout << "    headers are indented in relation to the switch block.\n";
	std::cout << std::endl;
	std::cout << "    --indent-cases  OR  -K\n";
	std::cout << "    Indent case blocks from the 'case XXX:' headers.\n";
	std::cout << "    Case statements not enclosed in blocks are NOT indented.\n";
	std::cout << std::endl;
	std::cout << "    --indent-namespaces  OR  -N\n";
	std::cout << "    Indent the contents of namespace blocks.\n";
	std::cout << std::endl;
	std::cout << "    --indent-after-parens  OR  -xU\n";
	std::cout << "    Indent, instead of align, continuation lines following lines\n";
	std::cout << "    that contain an opening paren '(' or an assignment '='. \n";
	std::cout << std::endl;
	std::cout << "    --indent-continuation=#  OR  -xt#\n";
	std::cout << "    Indent continuation lines an additional # indents.\n";
	std::cout << "    The valid values are 0 thru 4 indents.\n";
	std::cout << "    The default value is 1 indent.\n";
	std::cout << std::endl;
	std::cout << "    --indent-labels  OR  -L\n";
	std::cout << "    Indent labels so that they appear one indent less than\n";
	std::cout << "    the current indentation level, rather than being\n";
	std::cout << "    flushed completely to the left (which is the default).\n";
	std::cout << std::endl;
	std::cout << "    --indent-preproc-block  OR  -xW\n";
	std::cout << "    Indent preprocessor blocks at brace level 0.\n";
	std::cout << "    Without this option the preprocessor block is not indented.\n";
	std::cout << std::endl;
	std::cout << "    --indent-preproc-cond  OR  -xw\n";
	std::cout << "    Indent preprocessor conditional statements #if/#else/#endif\n";
	std::cout << "    to the same level as the source code.\n";
	std::cout << std::endl;
	std::cout << "    --indent-preproc-define  OR  -w\n";
	std::cout << "    Indent multi-line preprocessor #define statements.\n";
	std::cout << std::endl;
	std::cout << "    --indent-col1-comments  OR  -Y\n";
	std::cout << "    Indent line comments that start in column one.\n";
	std::cout << std::endl;
	std::cout << "    --indent-lambda\n";
	std::cout << "    Indent C++ lambda functions (experimental, broken for complex fct bodies)\n";
	std::cout << std::endl;
	std::cout << "    --min-conditional-indent=#  OR  -m#\n";
	std::cout << "    Indent a minimal # spaces in a continuous conditional\n";
	std::cout << "    belonging to a conditional header.\n";
	std::cout << "    The valid values are:\n";
	std::cout << "    0 - no minimal indent.\n";
	std::cout << "    1 - indent at least one additional indent.\n";
	std::cout << "    2 - indent at least two additional indents.\n";
	std::cout << "    3 - indent at least one-half an additional indent.\n";
	std::cout << "    The default value is 2, two additional indents.\n";
	std::cout << std::endl;
	std::cout << "    --max-continuation-indent=#  OR  -M#\n";
	std::cout << "    Indent a maximal # spaces in a continuation line,\n";
	std::cout << "    relative to the previous line.\n";
	std::cout << "    The minimum and default value is 40.\n";
	std::cout << std::endl;
	std::cout << "Padding Options:\n";
	std::cout << "----------------\n";
	std::cout << "    --break-blocks  OR  -f\n";
	std::cout << "    Insert empty lines around unrelated blocks, labels, classes, ...\n";
	std::cout << std::endl;
	std::cout << "    --break-blocks=all  OR  -F\n";
	std::cout << "    Like --break-blocks, except also insert empty lines \n";
	std::cout << "    around closing headers (e.g. 'else', 'catch', ...).\n";
	std::cout << std::endl;
	std::cout << "    --pad-oper  OR  -p\n";
	std::cout << "    Insert space padding around operators.\n";
	std::cout << std::endl;
	std::cout << "    --pad-comma  OR  -xg\n";
	std::cout << "    Insert space padding after commas.\n";
	std::cout << std::endl;
	std::cout << "    --pad-paren  OR  -P\n";
	std::cout << "    Insert space padding around parenthesis on both the outside\n";
	std::cout << "    and the inside.\n";
	std::cout << std::endl;
	std::cout << "    --pad-paren-out  OR  -d\n";
	std::cout << "    Insert space padding around parenthesis on the outside only.\n";
	std::cout << std::endl;
	std::cout << "    --pad-first-paren-out  OR  -xd\n";
	std::cout << "    Insert space padding around first parenthesis in a series on\n";
	std::cout << "    the outside only.\n";
	std::cout << std::endl;
	std::cout << "    --pad-paren-in  OR  -D\n";
	std::cout << "    Insert space padding around parenthesis on the inside only.\n";
	std::cout << std::endl;
	std::cout << "    --pad-empty-paren  OR  -xo\n";
	std::cout << "    Apply padding to empty pairs of parentheses; combine with other\n";
	std::cout << "    parenthesis padding options.\n";
	std::cout << std::endl;
	std::cout << "    --pad-header  OR  -H\n";
	std::cout << "    Insert space padding after paren headers (e.g. 'if', 'for'...).\n";
	std::cout << std::endl;
	std::cout << "    --unpad-paren  OR  -U\n";
	std::cout << "    Remove unnecessary space padding around parenthesis. This\n";
	std::cout << "    can be used in combination with the 'pad' options above.\n";
	std::cout << std::endl;

	std::cout << "    --pad-brackets\n";
	std::cout << "    Insert space padding around square brackets on both the outside\n";
	std::cout << "    and the inside (experimental).\n";
	std::cout << std::endl;
	std::cout << "    --unpad-brackets\n";
	std::cout << "    Remove unnecessary space padding around square brackets (experimental).\n";
	std::cout << std::endl;

	std::cout << "    --delete-empty-lines  OR  -xe\n";
	std::cout << "    Delete empty lines within a function or method.\n";
	std::cout << "    It will NOT delete lines added by the break-blocks options.\n";
	std::cout << std::endl;
	std::cout << "    --fill-empty-lines  OR  -E\n";
	std::cout << "    Fill empty lines with the white space of their\n";
	std::cout << "    previous lines.\n";
	std::cout << std::endl;
	std::cout << "    --squeeze-lines=#\n";
	std::cout << "    Remove superfluous empty lines exceeding the given number (experimental).\n";
	std::cout << std::endl;
	std::cout << "    --squeeze-ws\n";
	std::cout << "    Remove superfluous whitespace (experimental).\n";
	std::cout << std::endl;
	std::cout << "    --align-pointer=type    OR  -k1\n";
	std::cout << "    --align-pointer=middle  OR  -k2\n";
	std::cout << "    --align-pointer=name    OR  -k3\n";
	std::cout << "    Attach a pointer or reference operator (*, &, or ^) to either\n";
	std::cout << "    the operator type (left), middle, or operator name (right).\n";
	std::cout << "    To align the reference separately use --align-reference.\n";
	std::cout << std::endl;
	std::cout << "    --align-reference=none    OR  -W0\n";
	std::cout << "    --align-reference=type    OR  -W1\n";
	std::cout << "    --align-reference=middle  OR  -W2\n";
	std::cout << "    --align-reference=name    OR  -W3\n";
	std::cout << "    Attach a reference operator (&) to either\n";
	std::cout << "    the operator type (left), middle, or operator name (right).\n";
	std::cout << "    If not set, follow pointer alignment.\n";
	std::cout << std::endl;
	std::cout << "Formatting Options:\n";
	std::cout << "-------------------\n";
	std::cout << "    --break-closing-braces  OR  -y\n";
	std::cout << "    Break braces before closing headers (e.g. 'else', 'catch', ...).\n";
	std::cout << "    Use with --style=java, --style=kr, --style=stroustrup,\n";
	std::cout << "    --style=linux, or --style=1tbs.\n";
	std::cout << std::endl;
	std::cout << "    --break-elseifs  OR  -e\n";
	std::cout << "    Break 'else if()' statements into two different lines.\n";
	std::cout << std::endl;
	std::cout << "    --break-one-line-headers  OR  -xb\n";
	std::cout << "    Break one line headers (e.g. 'if', 'while', 'else', ...) from a\n";
	std::cout << "    statement residing on the same line.\n";
	std::cout << std::endl;
	std::cout << "    --add-braces  OR  -j\n";
	std::cout << "    Add braces to unbraced one line conditional statements.\n";
	std::cout << std::endl;
	std::cout << "    --add-one-line-braces  OR  -J\n";
	std::cout << "    Add one line braces to unbraced one line conditional\n";
	std::cout << "    statements.\n";
	std::cout << std::endl;
	std::cout << "    --remove-braces  OR  -xj\n";
	std::cout << "    Remove braces from a braced one line conditional statements.\n";
	std::cout << std::endl;
	std::cout << "    --break-return-type       OR  -xB\n";
	std::cout << "    --break-return-type-decl  OR  -xD\n";
	std::cout << "    Break the return type from the function name. Options are\n";
	std::cout << "    for the function definitions and the function declarations.\n";
	std::cout << std::endl;
	std::cout << "    --attach-return-type       OR  -xf\n";
	std::cout << "    --attach-return-type-decl  OR  -xh\n";
	std::cout << "    Attach the return type to the function name. Options are\n";
	std::cout << "    for the function definitions and the function declarations.\n";
	std::cout << std::endl;
	std::cout << "    --keep-one-line-blocks  OR  -O\n";
	std::cout << "    Don't break blocks residing completely on one line.\n";
	std::cout << std::endl;
	std::cout << "    --keep-one-line-statements  OR  -o\n";
	std::cout << "    Don't break lines containing multiple statements into\n";
	std::cout << "    multiple single-statement lines.\n";
	std::cout << std::endl;
	std::cout << "    --convert-tabs  OR  -c\n";
	std::cout << "    Convert tabs to the appropriate number of spaces.\n";
	std::cout << std::endl;
	std::cout << "    --close-templates  OR  -xy\n";
	std::cout << "    Close ending angle brackets on template definitions.\n";
	std::cout << std::endl;
	std::cout << "    --remove-comment-prefix  OR  -xp\n";
	std::cout << "    Remove the leading '*' prefix on multi-line comments and\n";
	std::cout << "    indent the comment text one indent.\n";
	std::cout << std::endl;
	std::cout << "    --max-code-length=#    OR  -xC#\n";
	std::cout << "    --break-after-logical  OR  -xL\n";
	std::cout << "    max-code-length=# will break the line if it exceeds more than\n";
	std::cout << "    # characters. The valid values are 50 thru 200.\n";
	std::cout << "    If the line contains logical conditionals they will be placed\n";
	std::cout << "    first on the new line. The option break-after-logical will\n";
	std::cout << "    cause the logical conditional to be placed last on the\n";
	std::cout << "    previous line.\n";
	std::cout << std::endl;
	std::cout << "    --mode=c\n";
	std::cout << "    Indent a C or C++ source file (this is the default).\n";
	std::cout << std::endl;
	std::cout << "    --mode=java\n";
	std::cout << "    Indent a Java source file.\n";
	std::cout << std::endl;
	std::cout << "    --mode=cs\n";
	std::cout << "    Indent a C# source file.\n";
	std::cout << std::endl;
	std::cout << "    --mode=objc\n";
	std::cout << "    Indent an Objective-C source file.\n";
	std::cout << std::endl;
	std::cout << "    --mode=js\n";
	std::cout << "    Indent a JavaScript source file (experimental).\n";
	std::cout << std::endl;
	std::cout << "Objective-C Options:\n";
	std::cout << "--------------------\n";
	std::cout << "    --pad-method-prefix  OR  -xQ\n";
	std::cout << "    Insert space padding after the '-' or '+' Objective-C\n";
	std::cout << "    method prefix.\n";
	std::cout << std::endl;
	std::cout << "    --unpad-method-prefix  OR  -xR\n";
	std::cout << "    Remove all space padding after the '-' or '+' Objective-C\n";
	std::cout << "    method prefix.\n";
	std::cout << std::endl;
	std::cout << "    --pad-return-type  OR  -xq\n";
	std::cout << "    Insert space padding after the Objective-C return type.\n";
	std::cout << std::endl;
	std::cout << "    --unpad-return-type  OR  -xr\n";
	std::cout << "    Remove all space padding after the Objective-C return type.\n";
	std::cout << std::endl;
	std::cout << "    --pad-param-type  OR  -xS\n";
	std::cout << "    Insert space padding after the Objective-C param type.\n";
	std::cout << std::endl;
	std::cout << "    --unpad-param-type  OR  -xs\n";
	std::cout << "    Remove all space padding after the Objective-C param type.\n";
	std::cout << std::endl;
	std::cout << "    --align-method-colon  OR  -xM\n";
	std::cout << "    Align the colons in an Objective-C method definition.\n";
	std::cout << std::endl;
	std::cout << "    --pad-method-colon=none    OR  -xP\n";
	std::cout << "    --pad-method-colon=all     OR  -xP1\n";
	std::cout << "    --pad-method-colon=after   OR  -xP2\n";
	std::cout << "    --pad-method-colon=before  OR  -xP3\n";
	std::cout << "    Add or remove space padding before or after the colons in an\n";
	std::cout << "    Objective-C method call.\n";
	std::cout << std::endl;
	std::cout << "Other Options:\n";
	std::cout << "--------------\n";
	std::cout << "    --suffix=####\n";
	std::cout << "    Append the suffix #### instead of '.orig' to original filename.\n";
	std::cout << std::endl;
	std::cout << "    --suffix=none  OR  -n\n";
	std::cout << "    Do not retain a backup of the original file.\n";
	std::cout << std::endl;
	std::cout << "    --recursive  OR  -r  OR  -R\n";
	std::cout << "    Process subdirectories recursively.\n";
	std::cout << std::endl;
	std::cout << "    --dry-run\n";
	std::cout << "    Perform a trial run with no changes made to check for formatting.\n";
	std::cout << std::endl;
	std::cout << "    --exclude=####\n";
	std::cout << "    Specify a file or directory #### to be excluded from processing.\n";
	std::cout << std::endl;
	std::cout << "    --ignore-exclude-errors  OR  -i\n";
	std::cout << "    Allow processing to continue if there are errors in the exclude=####\n";
	std::cout << "    options. It will display the unmatched excludes.\n";
	std::cout << std::endl;
	std::cout << "    --ignore-exclude-errors-x  OR  -xi\n";
	std::cout << "    Allow processing to continue if there are errors in the exclude=####\n";
	std::cout << "    options. It will NOT display the unmatched excludes.\n";
	std::cout << std::endl;
	std::cout << "    --errors-to-stdout  OR  -X\n";
	std::cout << "    Print errors and help information to standard-output rather than\n";
	std::cout << "    to standard-error.\n";
	std::cout << std::endl;
	std::cout << "    --preserve-date  OR  -Z\n";
	std::cout << "    Preserve the original file's date and time modified. The time\n";
	std::cout << "     modified will be changed a few micro seconds to force a compile.\n";
	std::cout << std::endl;
	std::cout << "    --verbose  OR  -v\n";
	std::cout << "    Verbose mode. Extra informational messages will be displayed.\n";
	std::cout << std::endl;
	std::cout << "    --formatted  OR  -Q\n";
	std::cout << "    Formatted display mode. Display only the files that have been\n";
	std::cout << "    formatted.\n";
	std::cout << std::endl;
	std::cout << "    --quiet  OR  -q\n";
	std::cout << "    Quiet mode. Suppress all output except error messages.\n";
	std::cout << std::endl;
	std::cout << "    --lineend=windows  OR  -z1\n";
	std::cout << "    --lineend=linux    OR  -z2\n";
	std::cout << "    --lineend=macold   OR  -z3\n";
	std::cout << "    Force use of the specified line end style. Valid options\n";
	std::cout << "    are windows (CRLF), linux (LF), and macold (CR).\n";
	std::cout << std::endl;
	std::cout << "Command Line Only:\n";
	std::cout << "------------------\n";
	std::cout << "    --options=####\n";
	std::cout << "    --options=none\n";
	std::cout << "    Specify a default option file #### to read and use.\n";
	std::cout << "    It must contain a file path and a file name.\n";
	std::cout << "    'none' disables the default option file.\n";
	std::cout << std::endl;
	std::cout << "    --project\n";
	std::cout << "    --project=####\n";
	std::cout << "    --project=none\n";
	std::cout << "    Specify a project option file #### to read and use.\n";
	std::cout << "    It must contain a file name only, without a directory path.\n";
	std::cout << "    The file should be included in the project top-level directory.\n";
	std::cout << "    The default file name is .astylerc or _astylerc.\n";
	std::cout << "    'none' disables the project or environment variable file.\n";
	std::cout << std::endl;
	std::cout << "    --ascii  OR  -I\n";
	std::cout << "    The displayed output will be ascii characters only.\n";
	std::cout << std::endl;
	std::cout << "    --version  OR  -V\n";
	std::cout << "    Print version number.\n";
	std::cout << std::endl;
	std::cout << "    --help  OR  -h  OR  -?\n";
	std::cout << "    Print this help message.\n";
	std::cout << std::endl;
	std::cout << "    --html  OR  -!\n";
	std::cout << "    Open the HTML help file \"astyle.html\" in the default browser.\n";
	std::cout << "    The documentation must be installed in the standard install path.\n";
	std::cout << std::endl;
	std::cout << "    --html=####\n";
	std::cout << "    Open a HTML help file in the default browser using the file path\n";
	std::cout << "    ####. The path may include a directory path and a file name, or a\n";
	std::cout << "    file name only. Paths containing spaces must be enclosed in quotes.\n";
	std::cout << std::endl;
	std::cout << "    --stdin=####\n";
	std::cout << "    Use the file path #### as input to single file formatting.\n";
	std::cout << "    This is a replacement for redirection.\n";
	std::cout << std::endl;
	std::cout << "    --stdout=####\n";
	std::cout << "    Use the file path #### as output from single file formatting.\n";
	std::cout << "    This is a replacement for redirection.\n";
	std::cout << std::endl;
	std::cout << std::endl;
}

/**
 * Process files in the fileNameVector.
 */
void ASConsole::processFiles()
{
	if (isVerbose)
		printVerboseHeader();

	clock_t startTime = clock();     // start time of file formatting

	// loop thru input fileNameVector and process the files
	for (const std::string& fileNameVectorName : fileNameVector)
	{
		getFilePaths(fileNameVectorName);

		// loop thru fileName vector formatting the files
		for (const std::string& file : fileName)
			formatFile(file);
	}

	// files are processed, display stats
	if (isVerbose)
		printVerboseStats(startTime);
}

// process options from the command line and option files
// build the vectors fileNameVector, excludeVector, optionsVector,
// projectOptionsVector and fileOptionsVector
void ASConsole::processOptions(const std::vector<std::string>& argvOptions)
{
	bool ok = true;
	bool optionFileRequired = false;
	bool shouldParseOptionFile = true;
	bool projectOptionFileRequired = false;
	bool shouldParseProjectOptionFile = true;
	std::string projectOptionArg;		// save for display

	// get command line options
	for (std::string arg : argvOptions)
	{
		if (isOption(arg, "-I")
		        || isOption(arg, "--ascii"))
		{
			useAscii = true;
			setlocale(LC_ALL, "C");		// use English decimal indicator
			localizer.setLanguageFromName("en");
		}
		else if (isOption(arg, "--options=none"))
		{
			optionFileRequired = false;
			shouldParseOptionFile = false;
			optionFileName = "";
		}
		else if (isParamOption(arg, "--options="))
		{
			optionFileName = getParam(arg, "--options=");
			standardizePath(optionFileName);
			optionFileName = getFullPathName(optionFileName);
			optionFileRequired = true;
		}
		else if (isOption(arg, "--project=none"))
		{
			projectOptionFileRequired = false;
			shouldParseProjectOptionFile = false;
			setProjectOptionFileName("");
		}
		else if (isParamOption(arg, "--project="))
		{
			projectOptionFileName = getParam(arg, "--project=");
			standardizePath(projectOptionFileName);
			projectOptionFileRequired = true;
			shouldParseProjectOptionFile = false;
			projectOptionArg = projectOptionFileName;
		}
		else if (isOption(arg, "--project"))
		{
			projectOptionFileName = ".astylerc";
			projectOptionFileRequired = true;
			shouldParseProjectOptionFile = false;
			projectOptionArg = projectOptionFileName;
		}
		else if (isOption(arg, "-h")
		         || isOption(arg, "--help")
		         || isOption(arg, "-?"))
		{
			printHelp();
			exit(EXIT_SUCCESS);
		}
		else if (isOption(arg, "-!")
		         || isOption(arg, "--html"))
		{
			launchDefaultBrowser();
			exit(EXIT_SUCCESS);
		}
		else if (isParamOption(arg, "--html="))
		{
			std::string htmlFilePath = getParam(arg, "--html=");
			launchDefaultBrowser(htmlFilePath.c_str());
			exit(EXIT_SUCCESS);
		}
		else if (isOption(arg, "-V")
		         || isOption(arg, "--version"))
		{
			printf("Artistic Style Version %s\n", g_version);
			exit(EXIT_SUCCESS);
		}
		else if (isParamOption(arg, "--stdin="))
		{
			std::string path = getParam(arg, "--stdin=");
			standardizePath(path);
			setStdPathIn(path);
		}
		else if (isParamOption(arg, "--stdout="))
		{
			std::string path = getParam(arg, "--stdout=");
			standardizePath(path);
			setStdPathOut(path);
		}
		else if (arg[0] == '-')
		{
			optionsVector.emplace_back(arg);
		}
		else // file-name
		{
			standardizePath(arg);
			fileNameVector.emplace_back(arg);
		}
	}

	// get option file path and name
	if (shouldParseOptionFile)
	{
		if (optionFileName.empty())
		{
			const char* const env = getenv("ARTISTIC_STYLE_OPTIONS");
			if (env != nullptr)
			{
				setOptionFileName(env);
				standardizePath(optionFileName);
				optionFileName = getFullPathName(optionFileName);
			}
		}
		// for Linux
		if (optionFileName.empty())
		{
			const char* const env = getenv("HOME");
			if (env != nullptr)
			{
				std::string name = std::string(env) + "/.astylerc";
				if (fileExists(name.c_str()))
					setOptionFileName(name);
			}
		}
		// for Windows
		if (optionFileName.empty())
		{
			const char* const env = getenv("APPDATA");
			if (env != nullptr)
			{
				std::string name = std::string(env) + "\\astylerc";
				if (fileExists(name.c_str()))
					setOptionFileName(name);
			}
		}

	}

	// find project option file
	if (projectOptionFileRequired)
	{
		std::string optfilepath = findProjectOptionFilePath(projectOptionFileName);
		if (optfilepath.empty() || projectOptionArg.empty())
			error(_("Cannot open project option file"), projectOptionArg.c_str());
		standardizePath(optfilepath);
		setProjectOptionFileName(optfilepath);
	}
	if (shouldParseProjectOptionFile)
	{
		const char* const env = getenv("ARTISTIC_STYLE_PROJECT_OPTIONS");
		if (env != nullptr)
		{
			std::string optfilepath = findProjectOptionFilePath(env);
			standardizePath(optfilepath);
			setProjectOptionFileName(optfilepath);
		}
	}

	ASOptions options(formatter, *this);
	if (!optionFileName.empty())
	{
		std::stringstream optionsIn;
		if (!fileExists(optionFileName.c_str()))
			error(_("Cannot open default option file"), optionFileName.c_str());
		FileEncoding encoding = readFile(optionFileName, optionsIn);
		// bypass a BOM, all BOMs have been converted to utf-8
		if (encoding == UTF_8BOM || encoding == UTF_16LE || encoding == UTF_16BE)
		{
			char buf[4];
			optionsIn.get(buf, 4);
			assert(strcmp(buf, "\xEF\xBB\xBF") == 0);
		}
		options.importOptions(optionsIn, fileOptionsVector);
		ok = options.parseOptions(fileOptionsVector,
		                          std::string(_("Invalid default options:")));
	}
	else if (optionFileRequired)
		error(_("Cannot open default option file"), optionFileName.c_str());

	if (!ok)
	{
		(*errorStream) << options.getOptionErrors();
		(*errorStream) << _("For help on options type 'astyle -h'") << std::endl;
		error();
	}

	if (!projectOptionFileName.empty())
	{
		std::stringstream projectOptionsIn;
		if (!fileExists(projectOptionFileName.c_str()))
			error(_("Cannot open project option file"), projectOptionFileName.c_str());
		FileEncoding encoding = readFile(projectOptionFileName, projectOptionsIn);
		// bypass a BOM, all BOMs have been converted to utf-8
		if (encoding == UTF_8BOM || encoding == UTF_16LE || encoding == UTF_16BE)
		{
			char buf[4];
			projectOptionsIn.get(buf, 4);
			assert(strcmp(buf, "\xEF\xBB\xBF") == 0);
		}
		options.importOptions(projectOptionsIn, projectOptionsVector);
		ok = options.parseOptions(projectOptionsVector,
		                          std::string(_("Invalid project options:")));
	}

	if (!ok)
	{
		(*errorStream) << options.getOptionErrors();
		(*errorStream) << _("For help on options type 'astyle -h'") << std::endl;
		error();
	}

	// parse the command line options vector for errors
	ok = options.parseOptions(optionsVector,
	                          std::string(_("Invalid command line options:")));
	if (!ok)
	{
		(*errorStream) << options.getOptionErrors();
		(*errorStream) << _("For help on options type 'astyle -h'") << std::endl;
		error();
	}
}

// remove a file and check for an error
void ASConsole::removeFile(const char* fileName_, const char* errMsg) const
{
	if (remove(fileName_) != 0)
	{
		if (errno == ENOENT)        // no file is OK
			errno = 0;
		if (errno)
		{
			perror("errno message");
			error(errMsg, fileName_);
		}
	}
}

// rename a file and check for an error
void ASConsole::renameFile(const char* oldFileName, const char* newFileName, const char* errMsg) const
{
	int result = rename(oldFileName, newFileName);
	if (result != 0)
	{
		// if file still exists the remove needs more time - retry
		if (errno == EEXIST)
		{
			errno = 0;
			waitForRemove(newFileName);
			result = rename(oldFileName, newFileName);
		}
		if (result != 0)
		{
			perror("errno message");
			error(errMsg, oldFileName);
		}
	}
}

// make sure file separators are correct type (Windows or Linux)
// remove ending file separator
// remove beginning file separator if requested and NOT a complete file path
void ASConsole::standardizePath(std::string& path, bool removeBeginningSeparator /*false*/) const
{
#ifdef __VMS
	struct FAB fab;
	struct NAML naml;
	char less[NAML$C_MAXRSS];
	char sess[NAM$C_MAXRSS];
	int r0_status;

	// If we are on a VMS system, translate VMS style filenames to unix
	// style.
	fab = cc$rms_fab;
	fab.fab$l_fna = (char*) -1;
	fab.fab$b_fns = 0;
	fab.fab$l_naml = &naml;
	naml = cc$rms_naml;
	strcpy(sess, path.c_str());
	naml.naml$l_long_filename = (char*) sess;
	naml.naml$l_long_filename_size = path.length();
	naml.naml$l_long_expand = less;
	naml.naml$l_long_expand_alloc = sizeof(less);
	naml.naml$l_esa = sess;
	naml.naml$b_ess = sizeof(sess);
	naml.naml$v_no_short_upcase = 1;
	r0_status = sys$parse(&fab);
	if (r0_status == RMS$_SYN)
	{
		error("File syntax error", path.c_str());
	}
	else
	{
		if (!$VMS_STATUS_SUCCESS(r0_status))
		{
			(void) lib$signal(r0_status);
		}
	}
	less[naml.naml$l_long_expand_size - naml.naml$b_ver] = '\0';
	sess[naml.naml$b_esl - naml.naml$b_ver] = '\0';
	if (naml.naml$l_long_expand_size > naml.naml$b_esl)
	{
		path = decc$translate_vms(less);
	}
	else
	{
		path = decc$translate_vms(sess);
	}
#endif /* __VMS */

	// make sure separators are correct type (Windows or Linux)
	for (size_t i = 0; i < path.length(); i++)
	{
		i = path.find_first_of("/\\", i);
		if (i == std::string::npos)
			break;
		path[i] = g_fileSeparator;
	}
	// remove beginning separator if requested
	if (removeBeginningSeparator && (path[0] == g_fileSeparator))
		path.erase(0, 1);
}

void ASConsole::printMsg(const char* msg, const std::string& data) const
{
	if (isQuiet)
		return;
	printf(msg, data.c_str());
}

void ASConsole::printSeparatingLine() const
{
	std::string line;
	for (size_t i = 0; i < 60; i++)
		line.append("-");
	printMsg("%s\n", line);
}

void ASConsole::printVerboseHeader() const
{
	assert(isVerbose);
	if (isQuiet)
		return;
	// get the date
	time_t lt;
	char str[20];
	lt = time(nullptr);
	struct tm* ptr = localtime(&lt);
	strftime(str, 20, "%x", ptr);
	// print the header
	// 60 is the length of the separator in printSeparatingLine()
	std::string header = "Artistic Style " + std::string(g_version);
	size_t numSpaces = 60 - header.length() - strlen(str);
	header.append(numSpaces, ' ');
	header.append(str);
	header.append("\n");
	printf("%s", header.c_str());
	// print option files
	if (!optionFileName.empty())
		printf(_("Default option file  %s\n"), optionFileName.c_str());

	if (!projectOptionFileName.empty())
		printf(_("Project option file  %s\n"), projectOptionFileName.c_str());
}

void ASConsole::printVerboseStats(clock_t startTime) const
{
	assert(isVerbose);
	if (isQuiet)
		return;
	if (hasWildcard)
		printSeparatingLine();
	std::string formatted = getNumberFormat(filesFormatted);
	std::string unchanged = getNumberFormat(filesUnchanged);
	printf(_(" %s formatted   %s unchanged   "), formatted.c_str(), unchanged.c_str());

	// show processing time
	clock_t stopTime = clock();
	double secs = (stopTime - startTime) / double(CLOCKS_PER_SEC);
	if (secs < 60)
	{
		if (secs < 2.0)
			printf("%.2f", secs);
		else if (secs < 20.0)
			printf("%.1f", secs);
		else
			printf("%.0f", secs);
		printf("%s", _(" seconds   "));
	}
	else
	{
		// show minutes and seconds if time is greater than one minute
		int min = (int) secs / 60;
		secs -= min * 60;
		// NOTE: lround is not supported by MinGW and Embarcadero
		int minsec = int(secs + .5);
		printf(_("%d min %d sec   "), min, minsec);
	}

	std::string lines = getNumberFormat(linesOut);
	printf(_("%s lines\n"), lines.c_str());
	printf("\n");
}

void ASConsole::sleep(int seconds) const
{
	clock_t endwait;
	endwait = clock_t(clock() + seconds * CLOCKS_PER_SEC);
	while (clock() < endwait) {}
}

bool ASConsole::stringEndsWith(const std::string& str, const std::string& suffix) const
{
	int strIndex = (int) str.length() - 1;
	int suffixIndex = (int) suffix.length() - 1;

	while (strIndex >= 0 && suffixIndex >= 0)
	{
		if (tolower(str[strIndex]) != tolower(suffix[suffixIndex]))
			return false;

		--strIndex;
		--suffixIndex;
	}
	// suffix longer than string
	if (strIndex < 0 && suffixIndex >= 0)
		return false;
	return true;
}

void ASConsole::updateExcludeVector(const std::string& suffixParam)
{
	excludeVector.emplace_back(suffixParam);
	standardizePath(excludeVector.back(), true);
	// do not use emplace_back on vector<bool> until supported by macOS
	excludeHitsVector.push_back(false);
}

int ASConsole::waitForRemove(const char* newFileName) const
{
	struct stat stBuf;
	int seconds;
	// sleep a max of 20 seconds for the remove
	for (seconds = 1; seconds <= 20; seconds++)
	{
		sleep(1);
		if (stat(newFileName, &stBuf) != 0)
			break;
	}
	errno = 0;
	return seconds;
}

// From The Code Project http://www.codeproject.com/string/wildcmp.asp
// Written by Jack Handy - jakkhandy@hotmail.com
// Modified to compare case insensitive for Windows
int ASConsole::wildcmp(const char* wild, const char* data) const
{
	const char* cp = nullptr;
	const char* mp = nullptr;
	bool cmpval;

	while ((*data) && (*wild != '*'))
	{
		if (!g_isCaseSensitive)
			cmpval = (tolower(*wild) != tolower(*data)) && (*wild != '?');
		else
			cmpval = (*wild != *data) && (*wild != '?');

		if (cmpval)
		{
			return 0;
		}
		wild++;
		data++;
	}

	while (*data)
	{
		if (*wild == '*')
		{
			if (!*++wild)
			{
				return 1;
			}
			mp = wild;
			cp = data + 1;
		}
		else
		{
			if (!g_isCaseSensitive)
				cmpval = (tolower(*wild) == tolower(*data) || (*wild == '?'));
			else
				cmpval = (*wild == *data) || (*wild == '?');

			if (cmpval)
			{
				wild++;
				data++;
			}
			else
			{
				wild = mp;
				data = cp++;
			}
		}
	}

	while (*wild == '*')
	{
		wild++;
	}
	return !*wild;
}

void ASConsole::writeFile(const std::string& fileName_, FileEncoding encoding, std::ostringstream& out) const
{
	// save date accessed and date modified of original file
	struct stat stBuf;
	bool statErr = false;
	if (stat(fileName_.c_str(), &stBuf) == -1)
		statErr = true;

	// create a backup
	if (!noBackup)
	{
		std::string origFileName = fileName_ + origSuffix;
		removeFile(origFileName.c_str(), "Cannot remove pre-existing backup file");
		renameFile(fileName_.c_str(), origFileName.c_str(), "Cannot create backup file");
	}

	// write the output file
	std::ofstream fout(fileName_.c_str(), std::ios::binary | std::ios::trunc);
	if (!fout)
		error("Cannot open output file", fileName_.c_str());
	if (encoding == UTF_16LE || encoding == UTF_16BE)
	{
		// convert utf-8 to utf-16
		bool isBigEndian = (encoding == UTF_16BE);
		size_t utf16Size = encode.utf16LengthFromUtf8(out.str().c_str(), out.str().length());
		char* utf16Out = new char[utf16Size];
		size_t utf16Len = encode.utf8ToUtf16(const_cast<char*>(out.str().c_str()),
		                                     out.str().length(), isBigEndian, utf16Out);
		assert(utf16Len <= utf16Size);
		fout << std::string(utf16Out, utf16Len);
		delete[] utf16Out;
	}
	else
		fout << out.str();

	fout.close();

	// change date modified to original file date
	// Embarcadero must be linked with cw32mt not cw32
	if (preserveDate)
	{
		if (!statErr)
		{
			struct utimbuf outBuf;
			outBuf.actime = stBuf.st_atime;
			// add ticks so 'make' will recognize a change
			// Visual Studio 2008 needs more than 1
			outBuf.modtime = stBuf.st_mtime + 10;
			if (utime(fileName_.c_str(), &outBuf) == -1)
				statErr = true;
		}
		if (statErr)
		{
			perror("errno message");
			(*errorStream) << "*********  Cannot preserve file date" << std::endl;
		}
	}
}

#else	// ASTYLE_LIB

//-----------------------------------------------------------------------------
// ASLibrary class
// used by shared object (DLL) calls
//-----------------------------------------------------------------------------

char16_t* ASLibrary::formatUtf16(const char16_t* pSourceIn,		// the source to be formatted
                                 const char16_t* pOptions,		// AStyle options
                                 fpError fpErrorHandler,		// error handler function
                                 fpAlloc fpMemoryAlloc) const	// memory allocation function)
{
	const char* utf8In = convertUtf16ToUtf8(pSourceIn);
	if (utf8In == nullptr)
	{
		fpErrorHandler(121, "Cannot convert input utf-16 to utf-8.");
		return nullptr;
	}
	const char* utf8Options = convertUtf16ToUtf8(pOptions);
	if (utf8Options == nullptr)
	{
		delete[] utf8In;
		fpErrorHandler(122, "Cannot convert options utf-16 to utf-8.");
		return nullptr;
	}
	// call the Artistic Style formatting function
	// cannot use the callers memory allocation here
	char* utf8Out = AStyleMain(utf8In,
	                           utf8Options,
	                           fpErrorHandler,
	                           ASLibrary::tempMemoryAllocation);
	// finished with these
	delete[] utf8In;
	delete[] utf8Options;
	utf8In = nullptr;
	utf8Options = nullptr;
	// AStyle error has already been sent
	if (utf8Out == nullptr)
		return nullptr;
	// convert text to wide char and return it
	char16_t* utf16Out = convertUtf8ToUtf16(utf8Out, fpMemoryAlloc);
	delete[] utf8Out;
	utf8Out = nullptr;
	if (utf16Out == nullptr)
	{
		fpErrorHandler(123, "Cannot convert output utf-8 to utf-16.");
		return nullptr;
	}
	return utf16Out;
}

// STATIC method to allocate temporary memory for AStyle formatting.
// The data will be converted before being returned to the calling program.
char* STDCALL ASLibrary::tempMemoryAllocation(unsigned long memoryNeeded)
{
	char* buffer = new (std::nothrow) char[memoryNeeded];
	return buffer;
}

/**
 * Convert utf-8 strings to utf16 strings.
 * Memory is allocated by the calling program memory allocation function.
 * The calling function must check for errors.
 */
char16_t* ASLibrary::convertUtf8ToUtf16(const char* utf8In, fpAlloc fpMemoryAlloc) const
{
	if (utf8In == nullptr)
		return nullptr;
	char* data = const_cast<char*>(utf8In);
	size_t dataSize = strlen(utf8In);
	bool isBigEndian = encode.getBigEndian();
	// return size is in number of CHARs, not char16_t
	size_t utf16Size = (encode.utf16LengthFromUtf8(data, dataSize) + sizeof(char16_t));
	char* utf16Out = fpMemoryAlloc((long) utf16Size);
	if (utf16Out == nullptr)
		return nullptr;
#ifdef NDEBUG
	encode.utf8ToUtf16(data, dataSize + 1, isBigEndian, utf16Out);
#else
	size_t utf16Len = encode.utf8ToUtf16(data, dataSize + 1, isBigEndian, utf16Out);
	assert(utf16Len == utf16Size);
#endif
	assert(utf16Size == (encode.utf16len(reinterpret_cast<char16_t*>(utf16Out)) + 1) * sizeof(char16_t));
	return reinterpret_cast<char16_t*>(utf16Out);
}

/**
 * Convert utf16 strings to utf-8.
 * The calling function must check for errors and delete the
 * allocated memory.
 */
char* ASLibrary::convertUtf16ToUtf8(const char16_t* utf16In) const
{
	if (utf16In == nullptr)
		return nullptr;
	char* data = reinterpret_cast<char*>(const_cast<char16_t*>(utf16In));
	// size must be in chars
	size_t dataSize = encode.utf16len(utf16In) * sizeof(char16_t);
	bool isBigEndian = encode.getBigEndian();
	size_t utf8Size = encode.utf8LengthFromUtf16(data, dataSize, isBigEndian) + 1;
	char* utf8Out = new (std::nothrow) char[utf8Size];
	if (utf8Out == nullptr)
		return nullptr;
#ifdef NDEBUG
	encode.utf16ToUtf8(data, dataSize + 1, isBigEndian, true, utf8Out);
#else
	size_t utf8Len = encode.utf16ToUtf8(data, dataSize + 1, isBigEndian, true, utf8Out);
	assert(utf8Len == utf8Size);
#endif
	assert(utf8Size == strlen(utf8Out) + 1);
	return utf8Out;
}

#endif	// ASTYLE_LIB

//-----------------------------------------------------------------------------
// ASOptions class
// used by both console and library builds
//-----------------------------------------------------------------------------

#ifdef ASTYLE_LIB
ASOptions::ASOptions(ASFormatter& formatterArg)
	: formatter(formatterArg)
{ }
#else
ASOptions::ASOptions(ASFormatter& formatterArg, ASConsole& consoleArg)
	: formatter(formatterArg), console(consoleArg)
{ }
#endif

/**
 * parse the options vector
 * optionsVector can be either a fileOptionsVector (option file),
 * a projectOptionsVector (project option file),
 * or an optionsVector (command line)
 *
 * @return        true if no errors, false if errors
 */
bool ASOptions::parseOptions(std::vector<std::string>& optionsVector, const std::string& errorInfo)
{
	std::vector<std::string>::iterator option;
	std::string arg;
	std::string subArg;
	optionErrors.clear();

	for (option = optionsVector.begin(); option != optionsVector.end(); ++option)
	{
		arg = *option;

		if (arg.compare(0, 2, "--") == 0)
			parseOption(arg.substr(2), errorInfo);
		else if (arg[0] == '-')
		{
			size_t i;

			for (i = 1; i < arg.length(); ++i)
			{
				if (i > 1
				        && isalpha((unsigned char) arg[i])
				        && arg[i - 1] != 'x')
				{
					// parse the previous option in subArg
					parseOption(subArg, errorInfo);
					subArg = "";
				}
				// append the current option to subArg
				subArg.append(1, arg[i]);
			}
			// parse the last option
			parseOption(subArg, errorInfo);
			subArg = "";
		}
		else
		{
			parseOption(arg, errorInfo);
			subArg = "";
		}
	}
	if (optionErrors.str().length() > 0)
		return false;
	return true;
}

void ASOptions::parseOption(const std::string& arg, const std::string& errorInfo)
{
	if (isOption(arg, "A1", "style=allman") || isOption(arg, "style=bsd") || isOption(arg, "style=break"))
	{
		formatter.setFormattingStyle(STYLE_ALLMAN);
	}
	else if (isOption(arg, "A2", "style=java") || isOption(arg, "style=attach"))
	{
		formatter.setFormattingStyle(STYLE_JAVA);
	}
	else if (isOption(arg, "A3", "style=k&r") || isOption(arg, "style=kr") || isOption(arg, "style=k/r"))
	{
		formatter.setFormattingStyle(STYLE_KR);
	}
	else if (isOption(arg, "A4", "style=stroustrup"))
	{
		formatter.setFormattingStyle(STYLE_STROUSTRUP);
	}
	else if (isOption(arg, "A5", "style=whitesmith"))
	{
		formatter.setFormattingStyle(STYLE_WHITESMITH);
	}
	else if (isOption(arg, "A15", "style=vtk"))
	{
		formatter.setFormattingStyle(STYLE_VTK);
	}
	else if (isOption(arg, "A6", "style=ratliff") || isOption(arg, "style=banner"))
	{
		formatter.setFormattingStyle(STYLE_RATLIFF);
	}
	else if (isOption(arg, "A7", "style=gnu"))
	{
		formatter.setFormattingStyle(STYLE_GNU);
	}
	else if (isOption(arg, "A8", "style=linux") || isOption(arg, "style=knf"))
	{
		formatter.setFormattingStyle(STYLE_LINUX);
	}
	else if (isOption(arg, "A9", "style=horstmann") || isOption(arg, "style=run-in"))
	{
		formatter.setFormattingStyle(STYLE_HORSTMANN);
	}
	else if (isOption(arg, "A10", "style=1tbs") || isOption(arg, "style=otbs"))
	{
		formatter.setFormattingStyle(STYLE_1TBS);
	}
	else if (isOption(arg, "A14", "style=google"))
	{
		formatter.setFormattingStyle(STYLE_GOOGLE);
	}
	else if (isOption(arg, "A16", "style=mozilla"))
	{
		formatter.setFormattingStyle(STYLE_MOZILLA);
	}
	else if (isOption(arg, "A17", "style=webkit"))
	{
		formatter.setFormattingStyle(STYLE_WEBKIT);
	}
	else if (isOption(arg, "A11", "style=pico"))
	{
		formatter.setFormattingStyle(STYLE_PICO);
	}
	else if (isOption(arg, "A12", "style=lisp") || isOption(arg, "style=python"))
	{
		formatter.setFormattingStyle(STYLE_LISP);
	}
	// must check for mode=cs before mode=c !!!
	else if (isOption(arg, "mode=cs"))
	{
		formatter.setSharpStyle();
		formatter.setModeManuallySet(true);
	}
	else if (isOption(arg, "mode=c"))
	{
		formatter.setCStyle();
		formatter.setModeManuallySet(true);
	}
	else if (isOption(arg, "mode=java"))
	{
		formatter.setJavaStyle();
		formatter.setModeManuallySet(true);
	}
	else if (isOption(arg, "mode=js"))
	{
		formatter.setJSStyle();
		formatter.setModeManuallySet(true);
	}
	else if (isOption(arg, "mode=objc"))
	{
		formatter.setObjCStyle();
		formatter.setModeManuallySet(true);
	}
	else if (isParamOption(arg, "t", "indent=tab="))
	{
		int spaceNum = 4;
		std::string spaceNumParam = getParam(arg, "t", "indent=tab=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			isOptionError(arg, errorInfo);
		else
		{
			formatter.setTabIndentation(spaceNum, false);
		}
	}
	else if (isOption(arg, "indent=tab"))
	{
		formatter.setTabIndentation(4);
	}
	else if (isParamOption(arg, "T", "indent=force-tab="))
	{
		int spaceNum = 4;
		std::string spaceNumParam = getParam(arg, "T", "indent=force-tab=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			isOptionError(arg, errorInfo);
		else
		{
			formatter.setTabIndentation(spaceNum, true);
		}
	}
	else if (isOption(arg, "indent=force-tab"))
	{
		formatter.setTabIndentation(4, true);
	}
	else if (isParamOption(arg, "xT", "indent=force-tab-x="))
	{
		int tabNum = 8;
		std::string tabNumParam = getParam(arg, "xT", "indent=force-tab-x=");
		if (tabNumParam.length() > 0)
			tabNum = atoi(tabNumParam.c_str());
		if (tabNum < 2 || tabNum > 20)
			isOptionError(arg, errorInfo);
		else
		{
			formatter.setForceTabXIndentation(tabNum);
		}
	}
	else if (isOption(arg, "indent=force-tab-x"))
	{
		formatter.setForceTabXIndentation(8);
	}
	else if (isParamOption(arg, "s", "indent=spaces="))
	{
		int spaceNum = 4;
		std::string spaceNumParam = getParam(arg, "s", "indent=spaces=");
		if (spaceNumParam.length() > 0)
			spaceNum = atoi(spaceNumParam.c_str());
		if (spaceNum < 2 || spaceNum > 20)
			isOptionError(arg, errorInfo);
		else
		{
			formatter.setSpaceIndentation(spaceNum);
		}
	}
	else if (isOption(arg, "indent=spaces"))
	{
		formatter.setSpaceIndentation(4);
	}
	else if (isParamOption(arg, "xt", "indent-continuation="))
	{
		int contIndent = 1;
		std::string contIndentParam = getParam(arg, "xt", "indent-continuation=");
		if (contIndentParam.length() > 0)
			contIndent = atoi(contIndentParam.c_str());
		if (contIndent < 0)
			isOptionError(arg, errorInfo);
		else if (contIndent > 4)
			isOptionError(arg, errorInfo);
		else
			formatter.setContinuationIndentation(contIndent);
	}
	else if (isParamOption(arg, "m", "min-conditional-indent="))
	{
		int minIndent = MINCOND_TWO;
		std::string minIndentParam = getParam(arg, "m", "min-conditional-indent=");
		if (minIndentParam.length() > 0)
			minIndent = atoi(minIndentParam.c_str());
		if (minIndent >= MINCOND_END)
			isOptionError(arg, errorInfo);
		else
			formatter.setMinConditionalIndentOption(minIndent);
	}
	else if (isParamOption(arg, "M", "max-continuation-indent="))
	{
		int maxIndent = 40;
		std::string maxIndentParam = getParam(arg, "M", "max-continuation-indent=");
		if (maxIndentParam.length() > 0)
			maxIndent = atoi(maxIndentParam.c_str());
		if (maxIndent < 40)
			isOptionError(arg, errorInfo);
		else
			formatter.setMaxContinuationIndentLength(maxIndent);
	}
	else if (isOption(arg, "N", "indent-namespaces"))
	{
		formatter.setNamespaceIndent(true);
	}
	else if (isOption(arg, "C", "indent-classes"))
	{
		formatter.setClassIndent(true);
	}
	else if (isOption(arg, "xG", "indent-modifiers"))
	{
		formatter.setModifierIndent(true);
	}
	else if (isOption(arg, "S", "indent-switches"))
	{
		formatter.setSwitchIndent(true);
	}
	else if (isOption(arg, "K", "indent-cases"))
	{
		formatter.setCaseIndent(true);
	}
	else if (isOption(arg, "xU", "indent-after-parens"))
	{
		formatter.setAfterParenIndent(true);
	}
	else if (isOption(arg, "L", "indent-labels"))
	{
		formatter.setLabelIndent(true);
	}
	else if (isOption(arg, "xW", "indent-preproc-block"))
	{
		formatter.setPreprocBlockIndent(true);
	}
	else if (isOption(arg, "w", "indent-preproc-define"))
	{
		formatter.setPreprocDefineIndent(true);
	}
	else if (isOption(arg, "xw", "indent-preproc-cond"))
	{
		formatter.setPreprocConditionalIndent(true);
	}
	else if (isOption(arg, "y", "break-closing-braces"))
	{
		formatter.setBreakClosingHeaderBracesMode(true);
	}
	else if (isOption(arg, "O", "keep-one-line-blocks"))
	{
		formatter.setBreakOneLineBlocksMode(false);
	}
	else if (isOption(arg, "o", "keep-one-line-statements"))
	{
		formatter.setBreakOneLineStatementsMode(false);
	}
	else if (isOption(arg, "P", "pad-paren"))
	{
		formatter.setParensOutsidePaddingMode(true);
		formatter.setParensInsidePaddingMode(true);
	}
	else if (isOption(arg, "d", "pad-paren-out"))
	{
		formatter.setParensOutsidePaddingMode(true);
	}
	else if (isOption(arg, "xd", "pad-first-paren-out"))
	{
		formatter.setParensFirstPaddingMode(true);
	}
	else if (isOption(arg, "xo", "pad-empty-paren"))
	{
		formatter.setEmptyParensPaddingMode(true);
	}
	else if (isOption(arg, "D", "pad-paren-in"))
	{
		formatter.setParensInsidePaddingMode(true);
	}
	else if (isOption(arg, "H", "pad-header"))
	{
		formatter.setParensHeaderPaddingMode(true);
	}
	else if (isOption(arg, "U", "unpad-paren"))
	{
		formatter.setParensUnPaddingMode(true);
	}
	else if (isOption(arg, "p", "pad-oper"))
	{
		formatter.setOperatorPaddingMode(true);
	}
	else if (isOption(arg, "xg", "pad-comma"))
	{
		formatter.setCommaPaddingMode(true);
	}
	else if (isOption(arg, "xe", "delete-empty-lines"))
	{
		formatter.setDeleteEmptyLinesMode(true);
	}
	else if (isParamOption(arg, "squeeze-lines="))
	{
		int keepEmptyLines = 2;
		std::string keepEmptyLinesParam = getParam(arg, "squeeze-lines=");
		if (keepEmptyLinesParam.length() > 0)
			keepEmptyLines = atoi(keepEmptyLinesParam.c_str());
		if (keepEmptyLines < 1)
			isOptionError(arg, errorInfo);
		else
			formatter.setSqueezeEmptyLinesNumber(keepEmptyLines);
	}
	else if (isOption(arg, "E", "fill-empty-lines"))
	{
		formatter.setEmptyLineFill(true);
	}
	else if (isOption(arg, "c", "convert-tabs"))
	{
		formatter.setTabSpaceConversionMode(true);
	}
	else if (isOption(arg, "xy", "close-templates"))
	{
		formatter.setCloseTemplatesMode(true);
	}
	else if (isOption(arg, "F", "break-blocks=all"))
	{
		formatter.setBreakBlocksMode(true);
		formatter.setBreakClosingHeaderBlocksMode(true);
	}
	else if (isOption(arg, "f", "break-blocks"))
	{
		formatter.setBreakBlocksMode(true);
	}
	else if (isOption(arg, "e", "break-elseifs"))
	{
		formatter.setBreakElseIfsMode(true);
	}
	else if (isOption(arg, "xb", "break-one-line-headers"))
	{
		formatter.setBreakOneLineHeadersMode(true);
	}
	else if (isOption(arg, "j", "add-braces"))
	{
		formatter.setAddBracesMode(true);
	}
	else if (isOption(arg, "J", "add-one-line-braces"))
	{
		formatter.setAddOneLineBracesMode(true);
	}
	else if (isOption(arg, "xj", "remove-braces"))
	{
		formatter.setRemoveBracesMode(true);
	}
	else if (isOption(arg, "Y", "indent-col1-comments"))
	{
		formatter.setIndentCol1CommentsMode(true);
	}
	else if (isOption(arg, "align-pointer=type"))
	{
		formatter.setPointerAlignment(PTR_ALIGN_TYPE);
	}
	else if (isOption(arg, "align-pointer=middle"))
	{
		formatter.setPointerAlignment(PTR_ALIGN_MIDDLE);
	}
	else if (isOption(arg, "align-pointer=name"))
	{
		formatter.setPointerAlignment(PTR_ALIGN_NAME);
	}
	else if (isParamOption(arg, "k"))
	{
		int align = 0;
		std::string styleParam = getParam(arg, "k");
		if (styleParam.length() > 0)
			align = atoi(styleParam.c_str());
		if (align < 1 || align > 3)
			isOptionError(arg, errorInfo);
		else if (align == 1)
			formatter.setPointerAlignment(PTR_ALIGN_TYPE);
		else if (align == 2)
			formatter.setPointerAlignment(PTR_ALIGN_MIDDLE);
		else if (align == 3)
			formatter.setPointerAlignment(PTR_ALIGN_NAME);
	}
	else if (isOption(arg, "align-reference=none"))
	{
		formatter.setReferenceAlignment(REF_ALIGN_NONE);
	}
	else if (isOption(arg, "align-reference=type"))
	{
		formatter.setReferenceAlignment(REF_ALIGN_TYPE);
	}
	else if (isOption(arg, "align-reference=middle"))
	{
		formatter.setReferenceAlignment(REF_ALIGN_MIDDLE);
	}
	else if (isOption(arg, "align-reference=name"))
	{
		formatter.setReferenceAlignment(REF_ALIGN_NAME);
	}
	else if (isParamOption(arg, "W"))
	{
		int align = 0;
		std::string styleParam = getParam(arg, "W");
		if (styleParam.length() > 0)
			align = atoi(styleParam.c_str());
		if (align < 0 || align > 3)
			isOptionError(arg, errorInfo);
		else if (align == 0)
			formatter.setReferenceAlignment(REF_ALIGN_NONE);
		else if (align == 1)
			formatter.setReferenceAlignment(REF_ALIGN_TYPE);
		else if (align == 2)
			formatter.setReferenceAlignment(REF_ALIGN_MIDDLE);
		else if (align == 3)
			formatter.setReferenceAlignment(REF_ALIGN_NAME);
	}
	else if (isParamOption(arg, "max-code-length="))
	{
		int maxLength = 50;
		std::string maxLengthParam = getParam(arg, "max-code-length=");
		if (maxLengthParam.length() > 0)
			maxLength = atoi(maxLengthParam.c_str());
		if (maxLength < 50)
			isOptionError(arg, errorInfo);
		else if (maxLength > 200)
			isOptionError(arg, errorInfo);
		else
			formatter.setMaxCodeLength(maxLength);
	}
	else if (isParamOption(arg, "xC"))
	{
		int maxLength = 50;
		std::string maxLengthParam = getParam(arg, "xC");
		if (maxLengthParam.length() > 0)
			maxLength = atoi(maxLengthParam.c_str());
		if (maxLength > 200)
			isOptionError(arg, errorInfo);
		else
			formatter.setMaxCodeLength(maxLength);
	}
	else if (isOption(arg, "xL", "break-after-logical"))
	{
		formatter.setBreakAfterMode(true);
	}
	else if (isOption(arg, "xc", "attach-classes"))
	{
		formatter.setAttachClass(true);
	}
	else if (isOption(arg, "xV", "attach-closing-while"))
	{
		formatter.setAttachClosingWhile(true);
	}
	else if (isOption(arg, "xk", "attach-extern-c"))
	{
		formatter.setAttachExternC(true);
	}
	else if (isOption(arg, "xn", "attach-namespaces"))
	{
		formatter.setAttachNamespace(true);
	}
	else if (isOption(arg, "xl", "attach-inlines"))
	{
		formatter.setAttachInline(true);
	}
	else if (isOption(arg, "xp", "remove-comment-prefix"))
	{
		formatter.setStripCommentPrefix(true);
	}
	else if (isOption(arg, "xB", "break-return-type"))
	{
		formatter.setBreakReturnType(true);
	}
	else if (isOption(arg, "xD", "break-return-type-decl"))
	{
		formatter.setBreakReturnTypeDecl(true);
	}
	else if (isOption(arg, "xf", "attach-return-type"))
	{
		formatter.setAttachReturnType(true);
	}
	else if (isOption(arg, "xh", "attach-return-type-decl"))
	{
		formatter.setAttachReturnTypeDecl(true);
	}
	// To avoid compiler limit of blocks nested too deep.
	else if (!parseOptionContinued(arg, errorInfo))
	{
		isOptionError(arg, errorInfo);
	}
}	// End of parseOption function

// Continuation of parseOption.
// To avoid compiler limit of blocks nested too deep.
// Return 'true' if the option was found and processed.
// Return 'false' if the option was not found.
bool ASOptions::parseOptionContinued(const std::string& arg, const std::string& errorInfo)
{
	// Objective-C options
	if (isOption(arg, "xQ", "pad-method-prefix"))
	{
		formatter.setMethodPrefixPaddingMode(true);
	}
	else if (isOption(arg, "xR", "unpad-method-prefix"))
	{
		formatter.setMethodPrefixUnPaddingMode(true);
	}
	else if (isOption(arg, "xq", "pad-return-type"))
	{
		formatter.setReturnTypePaddingMode(true);
	}
	else if (isOption(arg, "xr", "unpad-return-type"))
	{
		formatter.setReturnTypeUnPaddingMode(true);
	}
	else if (isOption(arg, "xS", "pad-param-type"))
	{
		formatter.setParamTypePaddingMode(true);
	}
	else if (isOption(arg, "xs", "unpad-param-type"))
	{
		formatter.setParamTypeUnPaddingMode(true);
	}
	else if (isOption(arg, "xM", "align-method-colon"))
	{
		formatter.setAlignMethodColon(true);
	}
	else if (isOption(arg, "xP0", "pad-method-colon=none"))
	{
		formatter.setObjCColonPaddingMode(COLON_PAD_NONE);
	}
	else if (isOption(arg, "xP1", "pad-method-colon=all"))
	{
		formatter.setObjCColonPaddingMode(COLON_PAD_ALL);
	}
	else if (isOption(arg, "xP2", "pad-method-colon=after"))
	{
		formatter.setObjCColonPaddingMode(COLON_PAD_AFTER);
	}
	else if (isOption(arg, "xP3", "pad-method-colon=before"))
	{
		formatter.setObjCColonPaddingMode(COLON_PAD_BEFORE);
	}

#ifdef ASTYLE_LIB
	// End of options used by GUI /////////////////////////////////////////////////////////////////
	else
	{
		return false;
	}
	return true;
#else
	// Options used by only console ///////////////////////////////////////////////////////////////
	else if (isOption(arg, "n", "suffix=none"))
	{
		console.setNoBackup(true);
	}
	else if (isParamOption(arg, "suffix="))
	{
		std::string suffixParam = getParam(arg, "suffix=");
		if (suffixParam.length() > 0)
		{
			console.setOrigSuffix(suffixParam);
		}
	}
	else if (isParamOption(arg, "exclude="))
	{
		std::string suffixParam = getParam(arg, "exclude=");
		if (suffixParam.length() > 0)
			console.updateExcludeVector(suffixParam);
	}
	else if (isOption(arg, "r", "R") || isOption(arg, "recursive"))
	{
		console.setIsRecursive(true);
	}
	else if (isOption(arg, "dry-run"))
	{
		console.setIsDryRun(true);
	}
	else if (isOption(arg, "Z", "preserve-date"))
	{
		console.setPreserveDate(true);
	}
	else if (isOption(arg, "v", "verbose"))
	{
		console.setIsVerbose(true);
	}
	else if (isOption(arg, "Q", "formatted"))
	{
		console.setIsFormattedOnly(true);
	}
	else if (isOption(arg, "q", "quiet"))
	{
		console.setIsQuiet(true);
	}
	else if (isOption(arg, "i", "ignore-exclude-errors"))
	{
		console.setIgnoreExcludeErrors(true);
	}
	else if (isOption(arg, "xi", "ignore-exclude-errors-x"))
	{
		console.setIgnoreExcludeErrorsAndDisplay(true);
	}
	else if (isOption(arg, "X", "errors-to-stdout"))
	{
		console.setErrorStream(&std::cout);
	}
	else if (isOption(arg, "lineend=windows"))
	{
		formatter.setLineEndFormat(LINEEND_WINDOWS);
	}
	else if (isOption(arg, "lineend=linux"))
	{
		formatter.setLineEndFormat(LINEEND_LINUX);
	}
	else if (isOption(arg, "lineend=macold"))
	{
		formatter.setLineEndFormat(LINEEND_MACOLD);
	}
	else if (isParamOption(arg, "z"))
	{
		int lineendType = 0;
		std::string lineendParam = getParam(arg, "z");
		if (lineendParam.length() > 0)
			lineendType = atoi(lineendParam.c_str());
		if (lineendType < 1 || lineendType > 3)
			isOptionError(arg, errorInfo);
		else if (lineendType == 1)
			formatter.setLineEndFormat(LINEEND_WINDOWS);
		else if (lineendType == 2)
			formatter.setLineEndFormat(LINEEND_LINUX);
		else if (lineendType == 3)
			formatter.setLineEndFormat(LINEEND_MACOLD);
	}
	else if (isOption(arg, "squeeze-ws"))
	{
		formatter.setSqueezeWhitespace(true);
	}
	else if (isOption(arg, "pad-brackets"))
	{
		formatter.setBracketsOutsidePaddingMode(true);
		formatter.setBracketsInsidePaddingMode(true);
	}
	else if (isOption(arg, "unpad-brackets"))
	{
		formatter.setBracketsUnPaddingMode(true);
	}
	else if (isOption(arg, "indent-lambda"))
	{
		formatter.setLambdaIndentation(true);
	}
	else
	{
		return false;
	}
	return true;
#endif
}	// End of parseOptionContinued function

// Parse options from the option file.
void ASOptions::importOptions(std::stringstream& in, std::vector<std::string>& optionsVector)
{
	char ch;
	bool isInQuote = false;
	char quoteChar = ' ';
	std::string currentToken;

	while (in)
	{
		currentToken = "";
		do
		{
			in.get(ch);
			if (in.eof())
				break;
			// treat '#' as line comments
			if (ch == '#')
				while (in)
				{
					in.get(ch);
					if (ch == '\n' || ch == '\r')
						break;
				}

			// break options on new-lines, tabs, commas, or spaces
			// remove quotes from output
			if (in.eof() || ch == '\n' || ch == '\r' || ch == '\t' || ch == ',')
				break;
			if (ch == ' ' && !isInQuote)
				break;
			if (ch == quoteChar && isInQuote)
				break;
			if (ch == '"' || ch == '\'')
			{
				isInQuote = true;
				quoteChar = ch;
				continue;
			}
			currentToken.append(1, ch);
		}
		while (in);

		if (currentToken.length() != 0)
			optionsVector.emplace_back(currentToken);
		isInQuote = false;
	}
}

std::string ASOptions::getOptionErrors() const
{
	return optionErrors.str();
}

std::string ASOptions::getParam(const std::string& arg, const char* op)
{
	return arg.substr(strlen(op));
}

std::string ASOptions::getParam(const std::string& arg, const char* op1, const char* op2)
{
	return isParamOption(arg, op1) ? getParam(arg, op1) : getParam(arg, op2);
}

bool ASOptions::isOption(const std::string& arg, const char* op)
{
	return arg == op;
}

bool ASOptions::isOption(const std::string& arg, const char* op1, const char* op2)
{
	return (isOption(arg, op1) || isOption(arg, op2));
}

void ASOptions::isOptionError(const std::string& arg, const std::string& errorInfo)
{
	if (optionErrors.str().length() == 0)
		optionErrors << errorInfo << std::endl;   // need main error message
	optionErrors << "\t" << arg << std::endl;
}

bool ASOptions::isParamOption(const std::string& arg, const char* option)
{
	bool retVal = arg.compare(0, strlen(option), option) == 0;
	// if comparing for short option, 2nd char of arg must be numeric
	if (retVal && strlen(option) == 1 && arg.length() > 1)
		if (!isdigit((unsigned char) arg[1]))
			retVal = false;
	return retVal;
}

bool ASOptions::isParamOption(const std::string& arg, const char* option1, const char* option2)
{
	return isParamOption(arg, option1) || isParamOption(arg, option2);
}

//----------------------------------------------------------------------------
// ASEncoding class
//----------------------------------------------------------------------------

// Return true if an int is big endian.
bool ASEncoding::getBigEndian() const
{
	char16_t word = 0x0001;
	char* byte = reinterpret_cast<char*>(&word);
	return (byte[0] ? false : true);
}

// Swap the two low order bytes of a 16 bit integer value.
int ASEncoding::swap16bit(int value) const
{
	return (((value & 0xff) << 8) | ((value & 0xff00) >> 8));
}

// Return the length of a utf-16 C string.
// The length is in number of char16_t.
size_t ASEncoding::utf16len(const utf16* utf16In) const
{
	size_t length = 0;
	while (*utf16In++ != '\0')
		length++;
	return length;
}

// Adapted from SciTE UniConversion.cxx.
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// Modified for Artistic Style by Jim Pattee.
// Compute the length of an output utf-8 file given a utf-16 file.
// Input inLen is the size in BYTES (not wchar_t).
size_t ASEncoding::utf8LengthFromUtf16(const char* utf16In, size_t inLen, bool isBigEndian) const
{
	size_t len = 0;
	size_t wcharLen = (inLen / 2) + (inLen % 2);
	const char16_t* uptr = reinterpret_cast<const char16_t*>(utf16In);
	for (size_t i = 0; i < wcharLen;)
	{
		size_t uch = isBigEndian ? swap16bit(uptr[i]) : uptr[i];
		if (uch < 0x80)
			len++;
		else if (uch < 0x800)
			len += 2;
		else if ((uch >= SURROGATE_LEAD_FIRST) && (uch <= SURROGATE_LEAD_LAST))
		{
			len += 4;
			i++;
		}
		else
			len += 3;
		i++;
	}
	return len;
}

// Adapted from SciTE Utf8_16.cxx.
// Copyright (C) 2002 Scott Kirkwood.
// Modified for Artistic Style by Jim Pattee.
// Convert a utf-8 file to utf-16.
size_t ASEncoding::utf8ToUtf16(char* utf8In, size_t inLen, bool isBigEndian, char* utf16Out) const
{
	int nCur = 0;
	ubyte* pRead = reinterpret_cast<ubyte*>(utf8In);
	utf16* pCur = reinterpret_cast<utf16*>(utf16Out);
	const ubyte* pEnd = pRead + inLen;
	const utf16* pCurStart = pCur;
	eState state = eStart;

	// the BOM will automatically be converted to utf-16
	while (pRead < pEnd)
	{
		switch (state)
		{
			case eStart:
				if ((0xF0 & *pRead) == 0xF0)
				{
					nCur = (0x7 & *pRead) << 18;
					state = eSecondOf4Bytes;
				}
				else if ((0xE0 & *pRead) == 0xE0)
				{
					nCur = (~0xE0 & *pRead) << 12;
					state = ePenultimate;
				}
				else if ((0xC0 & *pRead) == 0xC0)
				{
					nCur = (~0xC0 & *pRead) << 6;
					state = eFinal;
				}
				else
				{
					nCur = *pRead;
					state = eStart;
				}
				break;
			case eSecondOf4Bytes:
				nCur |= (0x3F & *pRead) << 12;
				state = ePenultimate;
				break;
			case ePenultimate:
				nCur |= (0x3F & *pRead) << 6;
				state = eFinal;
				break;
			case eFinal:
				nCur |= (0x3F & *pRead);
				state = eStart;
				break;
				// no default case is needed
		}
		++pRead;

		if (state == eStart)
		{
			int codePoint = nCur;
			if (codePoint >= SURROGATE_FIRST_VALUE)
			{
				codePoint -= SURROGATE_FIRST_VALUE;
				int lead = (codePoint >> 10) + SURROGATE_LEAD_FIRST;
				*pCur++ = static_cast<utf16>(isBigEndian ? swap16bit(lead) : lead);
				int trail = (codePoint & 0x3ff) + SURROGATE_TRAIL_FIRST;
				*pCur++ = static_cast<utf16>(isBigEndian ? swap16bit(trail) : trail);
			}
			else
				*pCur++ = static_cast<utf16>(isBigEndian ? swap16bit(codePoint) : codePoint);
		}
	}
	// return value is the output length in BYTES (not wchar_t)
	return (pCur - pCurStart) * 2;
}

// Adapted from SciTE UniConversion.cxx.
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// Modified for Artistic Style by Jim Pattee.
// Compute the length of an output utf-16 file given a utf-8 file.
// Return value is the size in BYTES (not wchar_t).
size_t ASEncoding::utf16LengthFromUtf8(const char* utf8In, size_t len) const
{
	size_t ulen = 0;
	size_t charLen;
	for (size_t i = 0; i < len;)
	{
		unsigned char ch = static_cast<unsigned char>(utf8In[i]);
		if (ch < 0x80)
			charLen = 1;
		else if (ch < 0x80 + 0x40 + 0x20)
			charLen = 2;
		else if (ch < 0x80 + 0x40 + 0x20 + 0x10)
			charLen = 3;
		else
		{
			charLen = 4;
			ulen++;
		}
		i += charLen;
		ulen++;
	}
	// return value is the length in bytes (not wchar_t)
	return ulen * 2;
}

// Adapted from SciTE Utf8_16.cxx.
// Copyright (C) 2002 Scott Kirkwood.
// Modified for Artistic Style by Jim Pattee.
// Convert a utf-16 file to utf-8.
size_t ASEncoding::utf16ToUtf8(char* utf16In, size_t inLen, bool isBigEndian,
                               bool firstBlock, char* utf8Out) const
{
	int nCur16 = 0;
	int nCur = 0;
	ubyte* pRead = reinterpret_cast<ubyte*>(utf16In);
	ubyte* pCur = reinterpret_cast<ubyte*>(utf8Out);
	const ubyte* pEnd = pRead + inLen;
	const ubyte* pCurStart = pCur;
	static eState state = eStart;	// state is retained for subsequent blocks
	if (firstBlock)
		state = eStart;

	// the BOM will automatically be converted to utf-8
	while (pRead < pEnd)
	{
		switch (state)
		{
			case eStart:
				if (pRead >= pEnd)
				{
					++pRead;
					break;
				}
				if (isBigEndian)
				{
					nCur16 = static_cast<utf16>(*pRead++ << 8);
					nCur16 |= static_cast<utf16>(*pRead);
				}
				else
				{
					nCur16 = *pRead++;
					nCur16 |= static_cast<utf16>(*pRead << 8);
				}
				if (nCur16 >= SURROGATE_LEAD_FIRST && nCur16 <= SURROGATE_LEAD_LAST)
				{
					++pRead;
					int trail;
					if (isBigEndian)
					{
						trail = static_cast<utf16>(*pRead++ << 8);
						trail |= static_cast<utf16>(*pRead);
					}
					else
					{
						trail = *pRead++;
						trail |= static_cast<utf16>(*pRead << 8);
					}
					nCur16 = (((nCur16 & 0x3ff) << 10) | (trail & 0x3ff)) + SURROGATE_FIRST_VALUE;
				}
				++pRead;

				if (nCur16 < 0x80)
				{
					nCur = static_cast<ubyte>(nCur16 & 0xFF);
					state = eStart;
				}
				else if (nCur16 < 0x800)
				{
					nCur = static_cast<ubyte>(0xC0 | (nCur16 >> 6));
					state = eFinal;
				}
				else if (nCur16 < SURROGATE_FIRST_VALUE)
				{
					nCur = static_cast<ubyte>(0xE0 | (nCur16 >> 12));
					state = ePenultimate;
				}
				else
				{
					nCur = static_cast<ubyte>(0xF0 | (nCur16 >> 18));
					state = eSecondOf4Bytes;
				}
				break;
			case eSecondOf4Bytes:
				nCur = static_cast<ubyte>(0x80 | ((nCur16 >> 12) & 0x3F));
				state = ePenultimate;
				break;
			case ePenultimate:
				nCur = static_cast<ubyte>(0x80 | ((nCur16 >> 6) & 0x3F));
				state = eFinal;
				break;
			case eFinal:
				nCur = static_cast<ubyte>(0x80 | (nCur16 & 0x3F));
				state = eStart;
				break;
				// no default case is needed
		}
		*pCur++ = static_cast<ubyte>(nCur);
	}
	return pCur - pCurStart;
}

//----------------------------------------------------------------------------

}   // namespace astyle

//----------------------------------------------------------------------------

using namespace astyle;

//----------------------------------------------------------------------------
// ASTYLE_JNI functions for Java library builds
//----------------------------------------------------------------------------

#ifdef ASTYLE_JNI

// called by a java program to get the version number
// the function name is constructed from method names in the calling java program
extern "C"  EXPORT
jstring STDCALL Java_AStyleInterface_AStyleGetVersion(JNIEnv* env, jclass)
{
	return env->NewStringUTF(g_version);
}

// called by a java program to format the source code
// the function name is constructed from method names in the calling java program
extern "C"  EXPORT
jstring STDCALL Java_AStyleInterface_AStyleMain(JNIEnv* env,
                                                jobject obj,
                                                jstring textInJava,
                                                jstring optionsJava)
{
	g_env = env;                                // make object available globally
	g_obj = obj;                                // make object available globally

	jstring textErr = env->NewStringUTF("");    // zero length text returned if an error occurs

	// get the method ID
	jclass cls = env->GetObjectClass(obj);
	g_mid = env->GetMethodID(cls, "ErrorHandler", "(ILjava/lang/String;)V");
	if (g_mid == nullptr)
	{
		std::cout << "Cannot find java method ErrorHandler" << std::endl;
		return textErr;
	}

	// convert jstring to char*
	const char* textIn = env->GetStringUTFChars(textInJava, nullptr);
	const char* options = env->GetStringUTFChars(optionsJava, nullptr);

	// call the C++ formatting function
	char* textOut = AStyleMain(textIn, options, javaErrorHandler, javaMemoryAlloc);
	// if an error message occurred it was displayed by errorHandler
	if (textOut == nullptr)
		return textErr;

	// release memory
	jstring textOutJava = env->NewStringUTF(textOut);
	delete[] textOut;
	env->ReleaseStringUTFChars(textInJava, textIn);
	env->ReleaseStringUTFChars(optionsJava, options);

	return textOutJava;
}

// Call the Java error handler
void STDCALL javaErrorHandler(int errorNumber, const char* errorMessage)
{
	jstring errorMessageJava = g_env->NewStringUTF(errorMessage);
	g_env->CallVoidMethod(g_obj, g_mid, errorNumber, errorMessageJava);
}

// Allocate memory for the formatted text
char* STDCALL javaMemoryAlloc(unsigned long memoryNeeded)
{
	// error condition is checked after return from AStyleMain
	char* buffer = new (std::nothrow) char[memoryNeeded];
	return buffer;
}

#endif	// ASTYLE_JNI

//----------------------------------------------------------------------------
// ASTYLE_LIB functions for library builds
//----------------------------------------------------------------------------

#ifdef ASTYLE_LIB

//----------------------------------------------------------------------------
// ASTYLE_LIB entry point for AStyleMainUtf16 library builds
//----------------------------------------------------------------------------
/*
* IMPORTANT Visual C DLL linker for WIN32 must have the additional options:
*           /EXPORT:AStyleMain=_AStyleMain@16
*           /EXPORT:AStyleMainUtf16=_AStyleMainUtf16@16
*           /EXPORT:AStyleGetVersion=_AStyleGetVersion@0
* No /EXPORT is required for x64
*/
extern "C" EXPORT char16_t* STDCALL AStyleMainUtf16(const char16_t* pSourceIn,	// the source to be formatted
                                                    const char16_t* pOptions,	// AStyle options
                                                    fpError fpErrorHandler,		// error handler function
                                                    fpAlloc fpMemoryAlloc)		// memory allocation function
{
	if (fpErrorHandler == nullptr)         // cannot display a message if no error handler
		return nullptr;

	if (pSourceIn == nullptr)
	{
		fpErrorHandler(101, "No pointer to source input.");
		return nullptr;
	}
	if (pOptions == nullptr)
	{
		fpErrorHandler(102, "No pointer to AStyle options.");
		return nullptr;
	}
	if (fpMemoryAlloc == nullptr)
	{
		fpErrorHandler(103, "No pointer to memory allocation function.");
		return nullptr;
	}
#ifndef _WIN32
	// check size of char16_t on Linux
	int sizeCheck = 2;
	if (sizeof(char16_t) != sizeCheck)
	{
		fpErrorHandler(104, "char16_t is not the correct size.");
		return nullptr;
	}
#endif

	ASLibrary library;
	char16_t* utf16Out = library.formatUtf16(pSourceIn, pOptions, fpErrorHandler, fpMemoryAlloc);
	return utf16Out;
}

//----------------------------------------------------------------------------
// ASTYLE_LIB entry point for library builds
//----------------------------------------------------------------------------
/*
 * IMPORTANT Visual C DLL linker for WIN32 must have the additional options:
 *           /EXPORT:AStyleMain=_AStyleMain@16
 *           /EXPORT:AStyleMainUtf16=_AStyleMainUtf16@16
 *           /EXPORT:AStyleGetVersion=_AStyleGetVersion@0
 * No /EXPORT is required for x64
 */
extern "C" EXPORT char* STDCALL AStyleMain(const char* pSourceIn,		// the source to be formatted
                                           const char* pOptions,		// AStyle options
                                           fpError fpErrorHandler,		// error handler function
                                           fpAlloc fpMemoryAlloc)		// memory allocation function
{
	if (fpErrorHandler == nullptr)         // cannot display a message if no error handler
		return nullptr;

	if (pSourceIn == nullptr)
	{
		fpErrorHandler(101, "No pointer to source input.");
		return nullptr;
	}
	if (pOptions == nullptr)
	{
		fpErrorHandler(102, "No pointer to AStyle options.");
		return nullptr;
	}
	if (fpMemoryAlloc == nullptr)
	{
		fpErrorHandler(103, "No pointer to memory allocation function.");
		return nullptr;
	}

	ASFormatter formatter;
	ASOptions options(formatter);

	std::vector<std::string> optionsVector;
	std::stringstream opt(pOptions);

	options.importOptions(opt, optionsVector);

	bool ok = options.parseOptions(optionsVector, "Invalid Artistic Style options:");
	if (!ok)
		fpErrorHandler(130, options.getOptionErrors().c_str());

	std::stringstream in(pSourceIn);
	ASStreamIterator<std::stringstream> streamIterator(&in);
	std::ostringstream out;
	formatter.init(&streamIterator);

	while (formatter.hasMoreLines())
	{
		out << formatter.nextLine();
		if (formatter.hasMoreLines())
			out << streamIterator.getOutputEOL();
		else
		{
			// this can happen if the file if missing a closing brace and break-blocks is requested
			if (formatter.getIsLineReady())
			{
				out << streamIterator.getOutputEOL();
				out << formatter.nextLine();
			}
		}
	}

	size_t textSizeOut = out.str().length();
	char* pTextOut = fpMemoryAlloc((long) textSizeOut + 1);     // call memory allocation function
	if (pTextOut == nullptr)
	{
		fpErrorHandler(120, "Allocation failure on output.");
		return nullptr;
	}

	strcpy(pTextOut, out.str().c_str());
#ifndef NDEBUG
	// The checksum is an assert in the console build and ASFormatter.
	// This error returns the incorrectly formatted file to the editor.
	// This is done to allow the file to be saved for debugging purposes.
	if (formatter.getChecksumDiff() != 0)
		fpErrorHandler(220,
		               "Checksum error.\n"
		               "The incorrectly formatted file will be returned for debugging.");
#endif
	return pTextOut;
}

extern "C" EXPORT const char* STDCALL AStyleGetVersion(void)
{
	return g_version;
}

// ASTYLECON_LIB is defined to exclude "main" from the test programs
#elif !defined(ASTYLECON_LIB)

//----------------------------------------------------------------------------
// main function for ASConsole build
//----------------------------------------------------------------------------

int main(int argc, char** argv)
{
	// create objects
	ASFormatter formatter;
	std::unique_ptr<ASConsole> console(new ASConsole(formatter));

	// process command line and option files
	// build the vectors fileNameVector, optionsVector, and fileOptionsVector
	std::vector<std::string> argvOptions;
	argvOptions = console->getArgvOptions(argc, argv);
	console->processOptions(argvOptions);

	// if no files have been given, use cin for input and cout for output
	if (!console->fileNameVectorIsEmpty())
		console->processFiles();
	else
		console->formatCinToCout();

	return EXIT_SUCCESS;
}

#endif	// ASTYLE_LIB
