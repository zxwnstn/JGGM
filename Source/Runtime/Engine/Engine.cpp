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

int FEngine::Run()
{
	while (!bRequestedExit)
	{

	}

	return 0;
}

FEngine* FEngine::CreateEngine(IAppInstance* AppInst)
{
	if (!GEngine)
	{
		GEngine = new FEngine(AppInst);
	}

	return GEngine;
}
