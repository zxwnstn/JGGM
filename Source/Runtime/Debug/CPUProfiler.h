#pragma once

#include "Core/Timer.h"
class FThread;
class FProfileThreadMain;

struct FProfileData
{
	double Start;
	double ElapsedTime;
	int64 StartCycle;
	FString EventName;
};

class SCPUProfiler
{
public:

	static SCPUProfiler& Get() { return Inst; }

	void Initialize();
	void ShutDown();
	void EnqueueEvent(FProfileData&& Data);

private:
	static SCPUProfiler Inst;
	union {
		FThread* ProfileThread;
		FProfileThreadMain* ProfileMain;
	};
	std::vector<FProfileData> DataBuffer;
	std::mutex DataBufferMutex;
	bool bUseThread;

	static const int32 MaxBufferingDataCount = 16;
};

class FScopedCycleCounter
{
public:
	FScopedCycleCounter(FString&& EventName)
	{
		Data.EventName = std::move(EventName);
		Data.Start = FTimer::GetElapsedTimeFromStart();
		Data.StartCycle = FTimer::GetNowCycle();
	}

	~FScopedCycleCounter()
	{
		Data.ElapsedTime = FTimer::DeltaTime(Data.StartCycle);
		SCPUProfiler::Get().EnqueueEvent(std::move(Data));
	}

private:
	FProfileData Data;
};

