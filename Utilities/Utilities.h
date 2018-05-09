#ifndef Utilities_h_4020d5868802479791f807104b3fcf6e
#define Utilities_h_4020d5868802479791f807104b3fcf6e

#include <string>
#include <thread>
#include "defs.h"

#if defined (USE_BOOST_LIBRARIES)
#include <boost/thread.hpp>
#endif

namespace mvs
{
	void OurSplitPath(const char *path, char *fulldir, char *base);
	long OurGetCurrentThreadId();
	std::string ReturnTimestamp(unsigned int fields);
	int LargeFileSystemFseek(FILE * stream, HLint64 offset, int origin);
	HLint64 LargeFileSystemFtell(FILE * stream);

	bool WcharToUtf8(std::string &dest, const wchar_t *src, size_t srcSize);
	std::string WcharToUtf8(const wchar_t *src);
	bool Utf8ToWchar(std::wstring &dest, const char *src, size_t srcSize);
	const std::wstring &AnyStringToWchar(std::wstring &dest, const char *src);
	const std::wstring &AnyStringToWchar(std::wstring &dest, const char *src, size_t srcSize);
	const std::wstring &AnyStringToWchar(std::wstring &dest, const wchar_t *src);
	const std::string &AnyStringToUtf8(std::string &dest, const char *src);
	const std::string &AnyStringToUtf8(std::string &dest, const wchar_t *src);

#if defined (USE_BOOST_LIBRARIES)
	long OurGetThreadId(boost::thread::id tid);
#else
	long OurGetThreadId(std::thread::id tid);
#endif

	enum TimeStampField
	{
		TSF_DayofWeek = 0x0001,
		TSF_Month = 0x0002,
		TSF_DayOfMonth = 0x0004,
		TSF_HHMMSS = 0x0008,
		TSF_All = 0xFFFF
	};
}

#endif	/*Utilities_h_4020d5868802479791f807104b3fcf6e*/
