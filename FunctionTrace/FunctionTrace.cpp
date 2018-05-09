#include <string.h>
#include <stdlib.h>

#if defined (_WIN32)
// The following "WIN32_LEAN_AND_MEAN" avoids the preprocessor's "fatal error C1189: #error : WinSock.h has already been included"
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#endif	/* _WIN32 */

#include "FunctionTrace.h"
#include "Utilities/Utilities.h"

namespace mvs
{
	IndentationData FunctionTrace::_IndentationList [MAX_TRACE_THREADS];
	time_t IndentationData::_indentation_base_time;

#if IOS_FREEZES
	SharedLock functiontraceMutex;
#endif

	FunctionTrace::FunctionTrace (const char *file, long line, const char *function)
	{
		char buffer [1024];
		char fulldir [MAXIMUM_PATH_LENGTH];
		static bool firstTime = true;

		if (firstTime)
		{
			firstTime = false;

			// Initialize the static list <_IndentationList>
			FunctionTrace::Init ();
		}

		strcpy (_file, file);

		// Extract the source file name without the path
		OurSplitPath (_file, fulldir, _sourcefile);

		_line = line;
		strcpy (_function, function);

		_threadId = OurGetCurrentThreadId ();
		int threadListIndex = GetThreadListIndex (_threadId);

		if (threadListIndex > -1)
		{
			sprintf (buffer, "%s> %s (%s:%ld)", GetIndentationString (_IndentationList [threadListIndex]._indentation), _function, _sourcefile, _line);
			LOGGER_LOGLINE (("functiontrace", buffer));

			_IndentationList [threadListIndex].Increment ();
		}
	}

	FunctionTrace::~FunctionTrace ()
	{
		char buffer [1024];

		int threadListIndex = GetThreadListIndex (_threadId);

		if (threadListIndex > -1)
		{
			--_IndentationList [threadListIndex]._indentation;

			sprintf (buffer, "%s< %s", GetIndentationString (_IndentationList [threadListIndex]._indentation), _function);
			LOGGER_LOGLINE (("functiontrace", buffer));
		}
	}

	void FunctionTrace::Init ()
	{
	}

	int FunctionTrace::GetThreadListIndex (int threadId)
	{
		int i;
		int lowestVacantIndex = -1;
#if IOS_FREEZES
		WriteLock w_lock (functiontraceMutex);
#endif

		for (i=0; i<MAX_TRACE_THREADS; ++i)
		{
			if (_IndentationList [i]._threadId == threadId)
				return i;

			if	(
					(lowestVacantIndex == -1)
					&&
					(_IndentationList [i].isAvailable ())
				)
				lowestVacantIndex = i;
		}

		// Did not find entry for the thread ID, create a new one and
		// return index to it
		if (lowestVacantIndex == MAX_TRACE_THREADS - 1)
		{
			// No more room for threads, perform cleanup.  If no more room,
			// return failure.
			lowestVacantIndex = PerformCleanup ();
			if (lowestVacantIndex == MAX_TRACE_THREADS - 1)
				return -1;
		}

		_IndentationList [lowestVacantIndex]._threadId = threadId;
		_IndentationList [lowestVacantIndex]._indentation = 0;

		return lowestVacantIndex;
	}

	int FunctionTrace::PerformCleanup ()
	{
		// TODO: Boost has no way of checking whether the thread corresponding to
		//       a thread ID is active.  If we had the thread object we could call
		//       thread::timed_join() (see http://stackoverflow.com/questions/1667420/how-can-i-tell-reliably-if-a-boost-thread-has-exited-its-run-method)
		//
		//       This means that the FunctionTrace table may never be accurately cleaned up.
		//       This is only a problem when debugging or leaving the function trace on for a
		//       very long time, not a practical problem.
		//
		// 1 March 2012: Remove all threads that were last active more than
		//               TRACE_THREAD_CLEANUP_TIME_DELTA_SECONDS seconds ago.
		int i;
		double diff = difftime (time (NULL), IndentationData::GetBaseTime ());
		long magicNumber = (long) diff - (long) TRACE_THREAD_CLEANUP_TIME_DELTA_SECONDS;
		int lowestVacantIndex = -1;

		for (i=0; i<MAX_TRACE_THREADS; ++i)
		{
			if (_IndentationList [i].isAvailable ())
			{
				// Vacant entry
				if (lowestVacantIndex == -1)
					lowestVacantIndex = i;
				continue;
			}

			if (_IndentationList [i].GetLastAccessDelta () < magicNumber)
			{
				// This entry has not been updated for a long period, garbage collect it
				_IndentationList [i].Init ();
			}
		}

		return lowestVacantIndex;
	}

	const char *FunctionTrace::GetIndentationString (int n)
	{
		static char gapString [2048];
		static bool firstTime = true;

		if (firstTime)
		{
			firstTime = false;

			memset (gapString, ' ', sizeof (gapString));
			gapString [sizeof (gapString) - 1] = 0;
		}

		// Prevent negative indentations, make gapString[] access exceed the end
		if (n < 0)
			n = 0;

		return gapString + sizeof (gapString) - 1 - n;
	}

	const char *FunctionTrace::GetIndentationString ()
	{
		int threadListIndex = GetThreadListIndex (_threadId);

		if (threadListIndex > -1)
		{
			return GetIndentationString (_IndentationList [threadListIndex]._indentation);
		}
		else
			return "";
	}

	const char *FunctionTrace::GetFunctionName ()
	{
		return _function;
	}
}
