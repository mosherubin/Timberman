#ifndef logger_h_95d88661616f4e1f8536ec1dd2aa1039
#define logger_h_95d88661616f4e1f8536ec1dd2aa1039

#include <vector>

#if defined (USE_BOOST_LIBRARIES)
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#else
#include <mutex>
#endif

#if defined(QNX) || defined(ABOX)
#include <stdarg.h>
#endif

// The following macros are meant to reduce the run-time execution time used by tracing
// statements.  It does this by separating the mask from the rest of the variable-length parameter
// list, and testing the mask before creating the (possibly) costly parameter list.
//
// For this scheme to work, these macros should be used, rather than the low-level Logger
// functions.
//
// The macro should be used in the following way:
//
//     LOGGER_LOGLINE (("some-mask", "Product version:    %ls", CONVERTFORANDROID(ALPRODUCT_VERSION)));
//
// Note that the variable-length parameter list has an additional set of parentheses around it.
//
// Note: We use "while ((void) 0, 0)" to avoid the infamous "warning C4127: conditional expression is constant" compilation message,
//       which is displayed when using "do {...} while (0)".  We need to use the do-while form rather than simply inserting within
//       {...} to enable code like this:
//
//           if (condition)
//               LOGGER_LOGLINE((...));
//           else
//               LOGGER_LOGLINE((...));
//
//       Something that fails compilation when using only {...}

#define LOGGER_LOGLINE(parms) \
	do \
	{ \
		int save_errno = errno; \
		errno = 0; \
		if (mvs::Logger::IsTracingEnabled ()) \
			mvs::Logger::LogLine parms; \
		errno = save_errno; \
	} \
	while ((void) 0, 0)

#define LOGGER_ASSERT(_Expression) if (mvs::Logger::IsTracingEnabled () && !(_Expression)) \
							   { \
							       mvs::Logger::LogLine ("assert", "LOGGER_ASSERT(%s) failed %s(%d)", #_Expression, __FILE__, __LINE__); \
							   }

#define HEXDUMPTRACE(parms) \
	do \
	{ \
	if (mvs::Logger::IsTracingEnabled ()) \
		mvs::Logger::HexDumpTrace parms; \
	} \
	while ((void) 0, 0)

namespace mvs
{
	class Logger
	{
	public:
		Logger ();
		~Logger ();

		static bool SetOutputFile (const char *utf8Path, bool restartLogFile = true);
		static bool GetOutputFile (std::string &path);

		static void SetTracingEnabled (bool tracingEnabled);
		static bool IsTracingEnabled ();
		static void Log (const char *mask, const char* format, ...);
		static void LogLine (const char *mask, const char* format, ...);
		static void LogLineNoArgs (const char *mask, const char* text, ...);
		static void LogLineWArgs (const char *mask, const char* format, va_list argptr);

		static void AddTraceMask (std::string &mask);
		static void AddTraceMask (const char *mask);
		static bool IsTraceMask (std::string &mask);
		static bool IsTraceMask (const char * mask);
		static void Cleanup ();
		static void HexDump(std::string& outBuffer, const char *ptr, size_t buflen, const char* format, ...);
		static void HexDumpTrace(const char *traceMask, const char* ptr, size_t buflen, const char *format, ...);
		/*DEBUG*/ static FILE *getLogFile () { return _logfile; }
		static void GetTraceMaskString (std::string &s);

	private:
		static FILE *_logfile;
		static std::string _utf8Path;
		static bool _initialized;
		static std::vector<std::string> _traceMaskList;
		static std::vector<std::string> _ignoreMaskList;
#if defined (USE_BOOST_LIBRARIES)
		static boost::mutex _mutex;
#else
		static std::mutex _mutex;
#endif

		static bool _tracingEnabled;

		static bool StartLogLine(const char *mask);
		static void DoLogLine (const char *mask, const char* format, va_list argptr, bool useArgPtr);
		static void DoLogLineWithArgs (const char* format, va_list argptr);

		static void OutputTimestamp ();
	};
}

#endif	/*logger_h_95d88661616f4e1f8536ec1dd2aa1039*/
