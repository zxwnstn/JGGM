#include "JGGMPch.h"

#include "AppInstance.h"
#include "Core/ThreadManager.h"

void IAppInstance::Initialize()
{
}

void IAppInstance::ShutDown()
{
}

void IAppInstance::Tick(float DeltaTime)
{
	
}

#if JGGM_EDITOR
void FEditorInstance::Initialize()
{
	IAppInstance::Initialize();
}

void FEditorInstance::ShutDown()
{
	IAppInstance::ShutDown();
}

void FEditorInstance::Tick(float DeltaTime)
{
	IAppInstance::Tick(DeltaTime);
}
#endif