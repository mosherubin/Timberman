#ifndef defs_h_3f09c9fc3143402eb9fbd11370d9ecbc
#define defs_h_3f09c9fc3143402eb9fbd11370d9ecbc

#define REFERTO(x) (void) x

#if defined (_WIN32)
	#define snprintf sprintf_s
	#define MAXIMUM_PATH_LENGTH	_MAX_PATH
	typedef __int64 HLint64;
	#define UTF16 unsigned __int16
	#define UTF32 unsigned __int32
#else
	#include <limits.h>
	#define MAXIMUM_PATH_LENGTH	PATH_MAX
	#define _stricmp strcasecmp
	typedef long long HLint64;
	#define UTF16 unsigned short
	#define UTF32 unsigned long
#endif	/*_WIN32*/

#if defined (_WIN32)
	#define PATH_SEPARATOR_CHAR_UTF8   '\\'
	#define PATH_SEPARATOR_CHAR_WIDE   L'\\'

	#define PATH_SEPARATOR_STRING_UTF8 "\\"
	#define PATH_SEPARATOR_STRING_WIDE L"\\"
#else
	#define PATH_SEPARATOR_CHAR_UTF8   '/'
	#define PATH_SEPARATOR_CHAR_WIDE   L'/'

	#define PATH_SEPARATOR_STRING_UTF8 "/"
	#define PATH_SEPARATOR_STRING_WIDE L"/"
#endif

#endif	/*defs_h_3f09c9fc3143402eb9fbd11370d9ecbc*/
