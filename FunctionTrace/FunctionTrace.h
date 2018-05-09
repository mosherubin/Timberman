#ifndef FunctionTrace_h_533e93a9b16a47239dc9b2d26b1181bd
#define FunctionTrace_h_533e93a9b16a47239dc9b2d26b1181bd

#include <stdlib.h>

#include "Logger/Logger.h"
#include "Utilities/Utilities.h"
#include "Utilities/defs.h"

#define FUNCTIONTRACE(funcname)	mvs::FunctionTrace __ft__ (__FILE__, __LINE__, funcname)

const int MAX_TRACE_THREADS = (4 * 1024);
const int TRACE_THREAD_CLEANUP_TIME_DELTA_SECONDS = (15 * 60);	/* 15 minutes in milliseconds */

// Trace macro: no arguments

#define TRACK_FUNCTION_CALL_0(func) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		func (); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

#define TRACK_FUNCTION_CALL_0_RET(ret, func) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		ret = func (); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

// Trace macro: one argument

#define TRACK_FUNCTION_CALL_1(func, arg_0) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		func (arg_0); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

#define TRACK_FUNCTION_CALL_1_RET(ret, func, arg_0) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		ret = func (arg_0); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

// Trace macro: two arguments

#define TRACK_FUNCTION_CALL_2(func, arg_0, arg_1) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		func (arg_0, arg_1); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

#define TRACK_FUNCTION_CALL_2_RET(ret, func, arg_0, arg_1) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		ret = func (arg_0, arg_1); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

// Trace macro: three arguments

#define TRACK_FUNCTION_CALL_3(func, arg_0, arg_1, arg_2) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		func (arg_0, arg_1, arg_2); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

#define TRACK_FUNCTION_CALL_3_RET(ret, func, arg_0, arg_1, arg_2) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		ret = func (arg_0, arg_1, arg_2); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

// Trace macro: four arguments

#define TRACK_FUNCTION_CALL_4(func, arg_0, arg_1, arg_2, arg_3) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		func (arg_0, arg_1, arg_2, arg_3); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

#define TRACK_FUNCTION_CALL_4_RET(ret, func, arg_0, arg_1, arg_2, arg_3) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		ret = func (arg_0, arg_1, arg_2, arg_3); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

// Trace macro: five arguments

#define TRACK_FUNCTION_CALL_5(func, arg_0, arg_1, arg_2, arg_3, arg_4) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		func (arg_0, arg_1, arg_2, arg_3, arg_4); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

#define TRACK_FUNCTION_CALL_5_RET(ret, func, arg_0, arg_1, arg_2, arg_3, arg_4) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		ret = func (arg_0, arg_1, arg_2, arg_3, arg_4); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

// Trace macro: six arguments

#define TRACK_FUNCTION_CALL_6(func, arg_0, arg_1, arg_2, arg_3, arg_4, arg_5) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		func (arg_0, arg_1, arg_2, arg_3, arg_4, arg_5); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

#define TRACK_FUNCTION_CALL_6_RET(ret, func, arg_0, arg_1, arg_2, arg_3, arg_4, arg_5) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		ret = func (arg_0, arg_1, arg_2, arg_3, arg_4, arg_5); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

// Trace macro: seven arguments

#define TRACK_FUNCTION_CALL_7(func, arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		func (arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

#define TRACK_FUNCTION_CALL_7_RET(ret, func, arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6) \
	do \
	{ \
		LOGGER_LOGLINE (("functiontrace", "%s> ---------- About to invoke callback function: %s", __ft__.GetIndentationString (), #func)); \
		ret = func (arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6); \
		LOGGER_LOGLINE (("functiontrace", "%s< ---------- After returning from callback function: %s", __ft__.GetIndentationString (), #func)); \
	} \
	while ((void) 0, 0)

namespace mvs
{
	class IndentationData
	{
	public:
		IndentationData ()
		{
			Init ();
		}

		~IndentationData ()
		{
		}

		void Init ()
		{
			_threadId = 0;
			_indentation = 0;
			_last_access_delta = 0;
			time (&_indentation_base_time);  /* get current time; same as: timer = time(NULL)  */
		}

		bool isAvailable ()
		{
			return	(
						(_threadId == 0)
						&&
						(_indentation == 0)
						&&
						(_last_access_delta == 0)
					);
		}

		void UpdateLastAccessDelta ()
		{
			double diff = difftime (time (NULL), _indentation_base_time);
			_last_access_delta = (long) diff;
		}

		void Increment ()
		{
			++_indentation;
			UpdateLastAccessDelta ();
		}

		void Decrement ()
		{
			--_indentation;
			UpdateLastAccessDelta ();
		}

		static time_t &GetBaseTime ()
		{
			return _indentation_base_time;
		}

		long &GetLastAccessDelta ()
		{
			return _last_access_delta;
		}

		int _threadId;
		int _indentation;
		static time_t _indentation_base_time;
		long _last_access_delta;
	};

	class FunctionTrace
	{
	public:
		FunctionTrace (const char *file, long line, const char* function);
		~FunctionTrace ();
		void Init ();
		int GetThreadListIndex (int threadId);
		int PerformCleanup ();
 		const char *GetIndentationString (int n);
		const char *GetIndentationString ();
		const char *GetFunctionName ();

	private:
		char _file [MAXIMUM_PATH_LENGTH];
		char _sourcefile [MAXIMUM_PATH_LENGTH];
		long _line;
		char _function [1024];
		int _threadId;
		static IndentationData _IndentationList [MAX_TRACE_THREADS];
	};
}

#endif	/*FunctionTrace_h_533e93a9b16a47239dc9b2d26b1181bd*/
