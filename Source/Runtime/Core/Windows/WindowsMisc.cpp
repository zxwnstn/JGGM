#include "JGGMPch.h"

#if _WIN32
namespace Inner {
	static int64 GFrequency;
	static int64 GStartCycle;
}

int64 PlatformTimerGetNowCycle()
{
	LARGE_INTEGER Now;
	QueryPerformanceCounter(&Now);

	return Now.QuadPart;
}

void PlatformTimerInitialize()
{
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);

	Inner::GFrequency = (int64)Frequency.QuadPart;
	Inner::GStartCycle = PlatformTimerGetNowCycle();
}

double PlatformTimerToMilliSecond(int64 Duration)
{
	return (Duration / (double)Inner::GFrequency);
}

double PlatformTimerElapsedTimeFromStart()
{
	return PlatformTimerToMilliSecond(PlatformTimerGetNowCycle() - Inner::GStartCycle);
}

void PlatformSleep(int32 MilliSecond)
{
	::Sleep(MilliSecond);
}

#endif
