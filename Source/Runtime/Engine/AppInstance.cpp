#include "JGGMPch.h"

#include "AppInstance.h"
#include "Core/ThreadManager.h"

void IAppInstance::Initialize()
{
}

void IAppInstance::Tick(float DeltaTime)
{
	
}

#if JGGM_EDITOR
void FEditorInstance::Initialize()
{
}



void FEditorInstance::Tick(float DeltaTime)
{
}
#endif