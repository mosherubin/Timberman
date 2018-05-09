#if defined (_WIN32)
// The following "WIN32_LEAN_AND_MEAN" avoids the preprocessor's "fatal error C1189: #error : WinSock.h has already been included"
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#else
#include <stdarg.h>
#endif	/* _WIN32 */

#include <cstring>
#include "Logger/Logger.h"
#include "Utilities/Utilities.h"
#include "utf8/utf8.h"

namespace mvs
{
	FILE *Logger::_logfile = 0;
	std::string Logger::_utf8Path;
	bool Logger::_initialized = false;
	std::vector <std::string> Logger::_traceMaskList;
	std::vector <std::string> Logger::_ignoreMaskList;
#if defined (USE_BOOST_LIBRARIES)
	boost::mutex Logger::_mutex;
#else
	std::mutex Logger::_mutex;
#endif
	bool Logger::_tracingEnabled = false;

	static char *outputBuffer = 0;
	static size_t outputBufferSize = 0;
	static size_t outputBufferSizeIncrement = (4 * 1024);

	bool Logger::SetOutputFile (const char *utf8Path, bool restartLogFile)
	{
#if defined (USE_BOOST_LIBRARIES)
		boost::mutex::scoped_lock lock (Logger::_mutex);
#else
		std::lock_guard<std::mutex> lock (Logger::_mutex);
#endif

		if (_logfile != 0)
		{
			// Close currently open output file
			fclose (_logfile);
			_logfile = 0;
		}

		_utf8Path = utf8Path;

		if (restartLogFile)//after wakeup we want to append to the same file
			_logfile = fopen (_utf8Path.c_str (), "w");
		else
			_logfile = fopen (_utf8Path.c_str (), "a");

		if (_logfile != 0)
		{
			fprintf (_logfile, "Starting logging ...\n");
			fprintf (_logfile, "--------------------\n");
			fprintf (_logfile, "\n");
			fflush(_logfile);
		}

		return (_logfile != 0);
	}

	bool Logger::GetOutputFile (std::string &path)
	{
		path.assign (_utf8Path);
		return true;
	}

	void Logger::SetTracingEnabled (bool tracingEnabled)
	{
		_tracingEnabled = tracingEnabled;
	}

	bool Logger::IsTracingEnabled ()
	{
		return _tracingEnabled;
	}

	void Logger::LogLineWArgs (const char *mask, const char* format, va_list argptr)
	{
		if ( StartLogLine(mask) )
			DoLogLine (mask, format, argptr, true);
	}

	void Logger::LogLine (const char *mask, const char* format, ...)
	{
#if defined (USE_BOOST_LIBRARIES)
		boost::mutex::scoped_lock lock (Logger::_mutex);
#else
		std::lock_guard<std::mutex> lock (Logger::_mutex);
#endif

		if ( StartLogLine(mask) )
		{
			va_list argptr;
			va_start (argptr, format);
			DoLogLine (mask, format, argptr, true);
			va_end (argptr);
		}
	}

	void Logger::LogLineNoArgs (const char *mask, const char* text, ...)
	{
#if defined (USE_BOOST_LIBRARIES)
		boost::mutex::scoped_lock lock (Logger::_mutex);
#else
		std::lock_guard<std::mutex> lock (Logger::_mutex);
#endif
		va_list dummyArgPtr;

		// Because Android defines the va_list data type differently than other platforms,
		// we need to pass a dummy va_list for _all_ platforms.  To avoid compiler warnings
		// we initialize <dummyArgPtr> but don't really use it.
		//
		// Note that the function prototype is variable length ("...") because gcc does
 		// not allow calling va_start() on a non-variable argument list function.

		va_start (dummyArgPtr, text);

		if (StartLogLine (mask))
			DoLogLine (mask, text, dummyArgPtr, false);

		va_end (dummyArgPtr);
	}

	bool Logger::StartLogLine(const char *mask)
	{
		if (_logfile == 0)
			return false;

		if (!Logger::IsTraceMask ((char *) mask))
			return false;

		return true;
	}

	void Logger::DoLogLine (const char *mask, const char* format, va_list argptr, bool useArgPtr)
	{
		// Implementation note:
		//     Originally the function used the value of <argptr> to decide whether
		//     to call DoLogLineWithArgs() or to write <format> out as-is, a significant
		//     savings in time.  Unfortunately, unlike other platforms that define va_list
		//     as "char *", Android considers va_list to be a built-in internal data type.
		//     Code that passes "(va_list) 0" to signify "don't call DoLogLineWithArgs()"
		//     fails to compile on Android.
		//
		//     Therefore, the solution is to pass a twin boolean parameter together with the
		//     <argptr> parameter.  The boolean's default value is "true', meaning "use <argptr>".
		//     Calling this function with <useArgPtr> set to false means "ignore <argptr>,
		//     don't call DoLogLineWithArgs()".

		const char *buffer = format;

		if (outputBuffer == 0)
		{
			outputBufferSize = (16 * 1024);
			outputBuffer = new char [outputBufferSize];
		}

		if (_logfile == 0)
		{
			// No log file has been defined, no point in writing anything out
			return;
		}

		OutputTimestamp ();

		if (useArgPtr)
		{
			DoLogLineWithArgs (format, argptr);
			buffer = outputBuffer;
		}

		// Make sure <_logfile> is still valid -- it may have been invalidated if the system closed down
		// while we were in the previous loop
		if (_logfile != 0)
		{
			// Display the trace mask
			fprintf (_logfile, "[%s] ", mask);

			// Display the trace message
			fprintf (_logfile, "%s", buffer);
			fprintf (_logfile, "\n");
			fflush (_logfile);
		}
	}

	void Logger::DoLogLineWithArgs (const char* format, va_list argptr)
	{
		int numCharsWritten = 0;

		for (;;)
		{
			numCharsWritten = vsnprintf (outputBuffer, outputBufferSize, format, argptr);

			if (numCharsWritten < 0)
			{
				// An error occurred in vsnprintf(), <outputBuffer> not large enough to receive the output, increase the buffer
				if (outputBufferSize >= (size_t) (200 * 1024))
				{
					// We've reached a 200K buffer, something is very wrong, do not increase the output buffer size
					fprintf (_logfile, "Error: vsnprintf() failed in Logger::DoLogLineWithArgs (return value is %d)", numCharsWritten);
					fprintf (_logfile, "       format = \"%s\"\n", format);
					return;
				}

				outputBufferSize += outputBufferSizeIncrement;
				delete[] outputBuffer;
				outputBuffer = new char [outputBufferSize];
			}
			else
			{
				// No error occurred, check the results
				if (numCharsWritten < (int) outputBufferSize)
				{
					// Successful call to vsnprintf(), the entire buffer was correctly rendered, leave the loop
					break;
				}
				else
				{
					// The buffer is not large enough for the full output, and <numCharsWritten> tells us
					// what the required output buffer size should be
					outputBufferSize = numCharsWritten + 1;
					delete[] outputBuffer;
					outputBuffer = new char [outputBufferSize];
				}
			}
		}
	}

	void Logger::AddTraceMask (std::string &mask)
	{
		if (mask.substr (0,1).compare ("-") == 0)
		{
			// This is a request to remove this mask from the mask list
			std::string trimmedMask = mask.substr (1);

			_ignoreMaskList.push_back (trimmedMask);
		}
		else
			_traceMaskList.push_back (mask);
	}

	void Logger::AddTraceMask (const char *mask)
	{
		std::string strMask = std::string (mask);

		AddTraceMask (strMask);
	}

	bool Logger::IsTraceMask (std::string &mask)
	{
		if (!Logger::_tracingEnabled)
			return false;

		return IsTraceMask (mask.c_str ());
	}

	bool Logger::IsTraceMask (const char * mask)
	{
		if (strlen (mask) == 0)
		{
			// An empty mask string always matches (i.e., always print)
			return true;
		}

		if (!Logger::_tracingEnabled)
			return false;

		std::vector<std::string>::iterator iter;

		if ( _traceMaskList.size() == 0 )
			return false;

		if (_ignoreMaskList.size() > 0)
		{
			// Check if the mask is in the ignore list
			for (iter=_ignoreMaskList.begin(); iter!=_ignoreMaskList.end(); ++iter)
			{
				if	(_stricmp ((*iter).c_str (), mask) == 0)
					return false;
			}
		}

		// If we reached here, the mask is not on the ignore list.  Check if
		// it is in the trace list.
		for (iter=_traceMaskList.begin(); iter!=_traceMaskList.end(); ++iter)
		{
			if	(
					(_stricmp ((*iter).c_str (), mask) == 0)
					||
					(_stricmp ((*iter).c_str (), "all") == 0)
				)
				return true;
		}

		return false;
	}

	void Logger::OutputTimestamp ()
	{
		// Display timestamp
		std::string s = ReturnTimestamp (TSF_HHMMSS);

		fprintf (_logfile, "%s (%10lu) ", s.c_str (), (unsigned long) OurGetCurrentThreadId ());
	}

	void Logger::Cleanup ()
	{
		// Called when the application is about to close, allowing closing files and
		// freeing resources.
#if defined (USE_BOOST_LIBRARIES)
		boost::mutex::scoped_lock lock (Logger::_mutex);
#else
		std::lock_guard<std::mutex> lock (Logger::_mutex);
#endif

		delete[] outputBuffer;
		outputBuffer = 0;

		if (_logfile != 0)
		{
			fclose (_logfile);
			_logfile = 0;
		}
	}

	void Logger::GetTraceMaskString (std::string &s)
	{
		s.clear ();

		std::vector<std::string>::iterator iter;

		for (iter=_traceMaskList.begin(); iter!=_traceMaskList.end(); ++iter)
		{
			if (s.size () > 0)
				s.append (" ");
			s.append (*iter);
		}

		for (iter=_ignoreMaskList.begin(); iter!=_ignoreMaskList.end(); ++iter)
		{
			if (s.size () > 0)
				s.append (" ");
			s.append ("-");
			s.append (*iter);
		}
	}

	// Found at http://stackoverflow.com/questions/29242/off-the-shelf-c-hex-dump-code

	void Logger::HexDump(std::string& outBuffer, const char *ptr, size_t buflen, const char* format, ...)
	{
		unsigned char *buf = (unsigned char*)ptr;
		size_t i, j;
		std::string line;
		char buffer[1024];

		outBuffer.clear();
		line.clear();

		// Create preamble from variable list
		char preamble[1024 * 16];
		va_list argptr;
		va_start(argptr, format);
		vsprintf(preamble, format, argptr);
		va_end(argptr);

		if (strlen(preamble) > 0)
			line += preamble;

		for (i = 0; i<buflen + 16; i += 16)
		{
			sprintf(buffer, "%06x: ", (unsigned int)i);
			line += buffer;

			for (j = 0; j<16; j++)
				if (i + j < buflen)
				{
					sprintf(buffer, "%02x ", buf[i + j]);
					line += buffer;
				}
				else
				{
					sprintf(buffer, "   ");
					line += buffer;
				}

			sprintf(buffer, " ");
			line += buffer;

			for (j = 0; j<16; j++)
				if (i + j < buflen)
				{
#ifdef QNX
					sprintf(buffer, "%c", std::isprint(buf[i + j]) ? buf[i + j] : '.');
#else
					sprintf(buffer, "%c", isprint(buf[i + j]) ? buf[i + j] : '.');
#endif
					line += buffer;
				}

			sprintf(buffer, "\n");
			line += buffer;

			outBuffer += line;
			line.clear();

			if (i + 16 >= buflen)
			{
				// We have no more bytes to dump, leave the loop
				break;
			}
		}
	}

	void Logger::HexDumpTrace(const char *traceMask, const char* ptr, size_t buflen, const char *format, ...)
	{
		if (IsTraceMask(traceMask))
		{
			std::string dump;

			// Create preamble from variable list
			char preamble[1024 * 16];
			va_list argptr;
			va_start(argptr, format);
			vsprintf(preamble, format, argptr);
			va_end(argptr);

			HexDump(dump, ptr, buflen, preamble);
			LogLineNoArgs(traceMask, dump.c_str());
		}
	}
}
