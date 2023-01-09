#include "JGGMPch.h"

#include "Engine.h"
#include "AppInstance.h"

FEngine* GEngine = nullptr;

FEngine::FEngine(IAppInstance* InAppInst)
	: AppInst(InAppInst)
	, bRequestedExit(false)
{
}

void FEngine::Initialize()
{
	AppInst->Initialize();
}

int32 FEngine::Tick()
{
	// Ticker
	float DeltaTime = 0.0f;

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
