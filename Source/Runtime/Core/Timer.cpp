#include "JGGMPch.h"

#include "Timer.h"

void FTimer::Initialize()
{
	extern void PlatformTimerInitialize();
	PlatformTimerInitialize();
}

int64 FTimer::GetNowCycle()
{
	extern int64 PlatformTimerGetNowCycle();
	return PlatformTimerGetNowCycle();
}

double FTimer::ToMilliSecond(int64 Duration)
{
	extern double PlatformTimerToMilliSecond(int64 Duration);
	return PlatformTimerToMilliSecond(Duration);
}

double FTimer::DeltaTime(int64 Last)
{
	return FTimer::ToMilliSecond(FTimer::GetNowCycle() - Last);
}

double FTimer::GetElapsedTimeFromStart()
{
	extern double PlatformTimerElapsedTimeFromStart();
	return PlatformTimerElapsedTimeFromStart();
}
