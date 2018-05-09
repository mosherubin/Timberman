#if defined (_WIN32)
// The following "WIN32_LEAN_AND_MEAN" avoids the preprocessor's "fatal error C1189: #error : WinSock.h has already been included"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <stdlib.h>
#include <libgen.h>
#include <wctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <climits>
#ifndef ANDROID
#include <ifaddrs.h>
#endif
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <vector>
#include <string>

#if !defined(_WIN32) && !defined(MAC) && !defined(IOS)
// The includes and defines within this block are only applicable to the GetAllIpsInternal()
// implementation for Linux platforms, i.e., not Windows, Mac OSX, or iOS
#if __MACH__ || __NetBSD__ || __OpenBSD__ || __FreeBSD__
#include <sys/sysctl.h>
#endif

/* Include sockio.h if needed */
#ifndef SIOCGIFCONF
#include <sys/sockio.h>
#endif

#include <netinet/in.h>
#include <net/if.h>
#include <netinet/if_ether.h>

#if __MACH__
#include <net/if_dl.h>
#endif

/* On platforms that have variable length
   ifreq use the old fixed length interface instead */
#ifdef OSIOCGIFCONF
#undef SIOCGIFCONF
#define SIOCGIFCONF OSIOCGIFCONF
#undef SIOCGIFADDR
#define SIOCGIFADDR OSIOCGIFADDR
#undef SIOCGIFBRDADDR
#define SIOCGIFBRDADDR OSIOCGIFBRDADDR
#endif
#endif	/* !defined(_WIN32) && !defined(MAC) && !defined(IOS) */
#endif	/* _WIN32 */

#include <stdio.h>
#include <wchar.h>
#include <sstream>
#include "Utilities/Utilities.h"
#include "FunctionTrace/FunctionTrace.h"
#include "Logger/Logger.h"
#include "utf8/utf8.h"
#ifdef QNX
#include <ctype.h>
#endif

/********************************************************************
*
*	Important Note!
*
*		The FunctionTrace system calls several functions in this
*		file.  These functions must not have FUNCTIONTRACE() in
*		them, as that will cause an infinite loop and subsequent
*		crash.
*
********************************************************************/

namespace mvs
{
#if defined (_WIN32)

	void OurSplitPath(const char *path, char *fulldir, char *base)
	{
		// NOTE: Do not insert a FUNCTIONTRACE() macro here, it will cause an infinite loop
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath(path, drive, dir, fname, ext);

		if (fulldir != 0)
		{
			strcpy(fulldir, drive);
			strcat(fulldir, dir);

			if (strlen(dir) > 1)
			{
				// Remove a trailing '\'
				char c = fulldir[strlen(fulldir) - 1];

				if ((c == '\\') || ((c == '/')))
					fulldir[strlen(fulldir) - 1] = 0;
			}
		}

		if (base != 0)
		{
			strcpy(base, fname);
			strcat(base, ext);
		}
	}
#else

	void OurSplitPath(const char *path, char *dir, char *base)
	{
		// NOTE: Do not insert a FUNCTIONTRACE() macro here, it will cause an infinite loop
		char resolved_path[MAXIMUM_PATH_LENGTH];
		char resolved_path_copy[MAXIMUM_PATH_LENGTH];

		if (dir != 0)
			strcpy(dir, "");

		if (base != 0)
			strcpy(base, "");

#if NEED_TO_FIX
		// TODO: Need to determine why this call fails on Android and Blackberry
		// Normalize the path
		if (realpath(path, resolved_path) == NULL)
			return;
#else
		strcpy(resolved_path, path);
#endif

		// Get directory path (note: dirname() modifies the input string,
		// so we need to work on a copy)
		strcpy(resolved_path_copy, resolved_path);
		const char *p = dirname((char *)resolved_path_copy);
		if (p == 0)
			return;
		if (dir != 0)
			strcpy(dir, p);

		// Get file name
		strcpy(resolved_path_copy, resolved_path);
		p = basename((char *)resolved_path_copy);
		if (p == 0)
			return;
		if (base != 0)
			strcpy(base, p);
	}
#endif	/*_WIN32*/

#if defined (USE_BOOST_LIBRARIES)
	long OurGetThreadId(boost::thread::id tid)
	{
		// NOTE: Inserting a FUNCTIONTRACE() macro here causes an endless loop, because
		//       the function trace code calls this function.

		std::stringstream ss;
		ss << tid;
		std::string idstr = ss.str();

		return strtoul(idstr.c_str(), 0, 16);
	}

	long OurGetCurrentThreadId()
	{
		// NOTE: Inserting a FUNCTIONTRACE() macro here causes an endless loop, because
		//       the function trace code calls this function.

		return OurGetThreadId(boost::this_thread::get_id());
	}
#else
	long OurGetThreadId(std::thread::id tid)
	{
		// NOTE: Inserting a FUNCTIONTRACE() macro here causes an endless loop, because
		//       the function trace code calls this function.

		std::stringstream ss;
		ss << tid;
		std::string idstr = ss.str();

		return strtoul(idstr.c_str(), 0, 16);
	}

	long OurGetCurrentThreadId()
	{
		// NOTE: Inserting a FUNCTIONTRACE() macro here causes an endless loop, because
		//       the function trace code calls this function.

		return OurGetThreadId(std::this_thread::get_id());
	}
#endif

	bool WcharToUtf8(std::string &dest, const wchar_t *src, size_t srcSize)
	{
		//FUNCTIONTRACE ("WcharToUtf8");	/* Commenting out the FUNCTIONTRACE() to reduce trace file noise */
		bool ret = true;

		dest.clear();

		size_t wideSize = sizeof(wchar_t);

		try
		{
			if (wideSize == 2)
			{
				utf8::utf16to8(src, src + srcSize, back_inserter(dest));
			}
			else if (wideSize == 4)
			{
				utf8::utf32to8(src, src + srcSize, back_inserter(dest));
			}
			else
			{
				// sizeof (wchar_t) is not 2 or 4 (does it equal one?)!  We didn't expect this and need to
				// write code to handle the case.
				LOGGER_LOGLINE(("error", "ERROR: sizeof(wchar_t) in WcharToUtf8 equals %d, something we don't support!", wideSize));
				ret = false;
			}
		}
		catch (utf8::exception e)
		{
			LOGGER_LOGLINE(("error", "utf8::exception (\"%s\") caught in WcharToUtf8()", e.what()));
			return false;
		}

		return ret;
	}

	std::string WcharToUtf8(const wchar_t *src)
	{
		// This function assumes the wchar_t array is NUL-terminated
		std::string result;

		WcharToUtf8(result, src, wcslen(src));

		return result;
	}

	bool Utf8ToWchar(std::wstring &dest, const char *src, size_t srcSize)
	{
		FUNCTIONTRACE("Utf8ToWchar");
		int wideSize = sizeof(wchar_t);
		bool ret = true;

		try
		{
			dest.clear();

			if (src == 0)
			{
				// A NULL pointer should return an empty string
				return true;
			}

			if (wideSize == 2)
			{
				utf8::utf8to16(src, src + srcSize, back_inserter(dest));
			}
			else if (wideSize == 4)
			{
				utf8::utf8to32(src, src + srcSize, back_inserter(dest));
			}
			else
			{
				// sizeof (wchar_t) is not 2 or 4 (does it equal one?)!  We didn't expect this and need to
				// write code to handle the case.
				LOGGER_LOGLINE(("error", "ERROR: sizeof(wchar_t) in Utf8ToWchar equals %d, something we don't support!", wideSize));
				ret = false;
			}
		}
		catch (utf8::exception e)
		{
			// This will happen if the last character(s) of the buffer are part of a UTF8 sequence.  In AnyLync, this
			// happens when a packet is part of, but not the end, of a JSON message structure.  In this case we will
			// just return false (for failure) and let the caller wait for the next packet(s).
			LOGGER_LOGLINE(("error", "utf8::exception (\"%s\") caught in Utf8ToWchar()", e.what()));
			return false;
		}

		return ret;
	}

	/**
	*	@brief		Convert std::string to std::wstring
	*  @details	This is one of two "thermos" function that magically
	*              convert either an std::string or std::wstring to an
	*              std::wstring.  This function converts string to wstring.
	*              Together with its std::string couterpart, a developer
	*              can use AnyStringToWchar() without having to worry
	*              whether the argument is UTF-8 or wide.
	*/
	const std::wstring &AnyStringToWchar(std::wstring &dest, const char *src)
	{
		FUNCTIONTRACE("AnyStringToWchar");
		Utf8ToWchar(dest, src, strlen(src));
		return dest;
	}

	/**
	*	@brief		Convert std::string to std::wstring
	*  @details	This is one of two "thermos" function that magically
	*              convert either an std::string or std::wstring to an
	*              std::wstring.  This function converts string to wstring.
	*              Together with its std::string couterpart, a developer
	*              can use AnyStringToWchar() without having to worry
	*              whether the argument is UTF-8 or wide.
	*				This function also use a provided length of source string
	*				which may be not null-terminated
	*/
	const std::wstring &AnyStringToWchar(std::wstring &dest, const char *src, size_t srcSize)
	{
		FUNCTIONTRACE("AnyStringToWchar");
		Utf8ToWchar(dest, src, srcSize);
		return dest;
	}

	/**
	*	@brief		Convert std::wstring to std::wstring
	*  @details	This is one of two "thermos" function that magically
	*              convert either an std::string or std::wstring to an
	*              std::wstring.  This function actually does nothing other than copy
	*              <src> to <dest>.  It is included for completeness, to enable a
	*              developer to use AnyStringToWchar() without having to worry
	*              whether the argument is UTF-8 or wide.
	*/
	const std::wstring &AnyStringToWchar(std::wstring &dest, const wchar_t *src)
	{
		FUNCTIONTRACE("AnyStringToWchar");

		dest.clear();

		if (src != 0)
			dest.assign(src);

		return dest;
	}

	/**
	*	@brief		Convert std::string to std::string
	*/
	const std::string &AnyStringToUtf8(std::string &dest, const char *src)
	{
		FUNCTIONTRACE("AnyStringToUtf8");

		dest.clear();

		if (src != 0)
			dest.assign(src);

		return dest;
	}

	/**
	*	@brief		Convert std::wstring to std::string
	*/
	const std::string &AnyStringToUtf8(std::string &dest, const wchar_t *src)
	{
		FUNCTIONTRACE("AnyStringToUtf8");
		WcharToUtf8(dest, src, wcslen(src));
		return dest;
	}

	std::string ReturnTimestamp(unsigned int fields)
	{
		// NOTE: Do not insert a FUNCTIONTRACE() macro here, it will cause an infinite loop
		struct tm *ptr;
		time_t tm;
		char buffer[1024];
		char dayofweek[128];
		char month[128];
		char dayofmonth[128];
		char hhmmss[128];
		std::string timestamp;

		tm = time(NULL);
		ptr = localtime(&tm);
		strcpy(buffer, asctime(ptr));

		// Remove trailing newline
		buffer[strlen(buffer) - 1] = 0;

		// Get pointer to hh:mm:ss only
		if (sscanf(buffer, "%s %s %s %s", dayofweek, month, dayofmonth, hhmmss) == EOF)
		{
			timestamp.assign("BAD_TIMESTAMP");
			return timestamp;
		}

		if (fields & TSF_DayofWeek)
		{
			if (timestamp.size() > 0)
				timestamp.append(" ");
			timestamp.append(dayofweek);
		}

		if (fields & TSF_Month)
		{
			if (timestamp.size() > 0)
				timestamp.append(" ");
			timestamp.append(month);
		}

		if (fields & TSF_DayOfMonth)
		{
			if (timestamp.size() > 0)
				timestamp.append(" ");
			timestamp.append(dayofmonth);
		}

		if (fields & TSF_HHMMSS)
		{
			if (timestamp.size() > 0)
				timestamp.append(" ");
			timestamp.append(hhmmss);
		}

		return timestamp;
	}

	/****************************************************************
	*
	*	LargeFileSystemFseek
	*
	****************************************************************/
	int LargeFileSystemFseek(FILE * stream, HLint64 offset, int origin)
	{
		FUNCTIONTRACE("LargeFileSystemFseek");

		int ret;

#if defined (_WIN32)
		ret = _fseeki64(stream, (__int64)offset, origin);
#elif defined (ANDROID)
		// Unlike all other platforms, lseek64() returns -1 for failure, and >=0 for success (i.e., the offset).
		// This differs from other platforms that always return 0 for success, hence the more complicated code
		// for Android.
		int fd = fileno(stream);
		off64_t res = lseek64(fd, (off64_t)offset, origin);
		ret = (res < 0) ? -1 : 0;
		if (ret != 0)
		{
			LOGGER_LOGLINE(("debug", "lseek64() failed, errno=%d in LargeFileSystemFseek", errno));
		}
#elif defined (IOS) || defined (MAC)
		ret = fseeko(stream, (off_t)offset, origin);
#else
		ret = fseeko64(stream, (off64_t)offset, origin);
#endif

		return ret;
	}

	/****************************************************************
	*
	*	LargeFileSystemFtell
	*
	****************************************************************/
	HLint64 LargeFileSystemFtell(FILE * stream)
	{
		FUNCTIONTRACE("LargeFileSystemFtell");

		HLint64 ret;

#if defined (_WIN32)
		ret = _ftelli64(stream);
#elif defined (ANDROID)
		// Paradoxically, NDK does not have a 64-bit tell() function.  Therefore, there is
		// no function we can call.  A tell()-like function is needed to determine the size
		// of a file, done by opeing the file, seeking to the end, then calling tell to get
		// the file size.  If an Android project needs to know the file size, it can invoke
		// a callback function in the client, passing the file path.
		return -1;
#elif defined (IOS) || defined (MAC)
		ret = ftello(stream);
#else
		ret = ftello64(stream);
#endif

		return ret;
	}
}
