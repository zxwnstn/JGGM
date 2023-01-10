#include "JGGMPch.h"

#include "Engine.h"
#include "AppInstance.h"
#include "Core/ThreadManager.h"
#include "Core/Timer.h"
#include "Debug/CPUProfiler.h"

FEngine* GEngine = nullptr;

FEngine::FEngine(IAppInstance* InAppInst)
	: AppInst(InAppInst)
	, bRequestedExit(false)
{
}

void FEngine::Initialize()
{
	SThreadManager::Get().Initialize();

	AppInst->Initialize();
	SCPUProfiler::Get().Initialize();

}

void FEngine::ShutDown()
{
	SThreadManager::Get().ShutDown();
	AppInst->ShutDown();
	SCPUProfiler::Get().ShutDown();
}

int32 FEngine::Tick()
{
	static int64 LastCycle = 0;
	float DeltaTime = 0.0f;
	if (LastCycle == 0)
	{
		LastCycle = FTimer::GetNowCycle();
	}
	else
	{
		DeltaTime = (float)FTimer::DeltaTime(LastCycle);
		LastCycle = FTimer::GetNowCycle();
	}

	AppInst->Tick(DeltaTime);

	extern void ThreadExperimentalWork();
	ThreadExperimentalWork();

	return 0;
}

int32 FEngine::Run()
{
	int32 ExitCode = 0;
	while (!bRequestedExit)
	{
		ExitCode = Tick();
	}

	return ExitCode;
}

FEngine* FEngine::CreateEngine(IAppInstance* AppInst)
{
	if (!GEngine)
	{
		GEngine = new FEngine(AppInst);
	}

	return GEngine;
}
