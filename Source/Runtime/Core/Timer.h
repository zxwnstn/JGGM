#pragma once

class FTimer
{
public:
	static void Initialize();
	static int64 GetNowCycle();

	static double ToMilliSecond(int64 Duration);
	static double DeltaTime(int64 Last);
	static double GetElapsedTimeFromStart();
};

