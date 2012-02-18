/*
 * A quick hack to get rid of my last dependency on Boost.
 * clock() in <ctime> doesn't give the resolution I want
 * on Linux. Need to link with rt on linux for the timer, though.
 */

#ifndef MTIMER_HPP
#define MTIMER_HPP

#ifdef _MSC_VER
	/*
	 * I have *no idea* if the Windows code works.
	 * Someone please test this. Tell me if it works,
	 * tell me how to fix it if it doesn't :)
	 */
	#include "windows.h"
	class MTimer {
	public:
		MTimer() : started(GetTickCount()) {}
		double elapsed() {
			return (GetTickCount()-started)/1000.0;
		}
	private:
		DWORD started;
	};
#else
	#include <sys/time.h>
	class MTimer {
	public:
		MTimer() {
			clock_gettime(CLOCK_MONOTONIC, &started);
		}
		double elapsed() {
			timespec temp;
			clock_gettime(CLOCK_MONOTONIC, &temp);
			return (double)(temp.tv_sec - started.tv_sec)+(double)((temp.tv_nsec-started.tv_nsec))/1.0E9;
		}
	private:
		timespec started;
	};
#endif

#endif
