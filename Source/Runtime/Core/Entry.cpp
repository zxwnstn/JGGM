#include "JGGMPch.h"

#include "Engine/Engine.h"
#include "Engine/AppInstance.h"

int32 JGGMMain()
{
	IAppInstance* AppInstance;
#if JGGM_EDITOR
	AppInstance = new FEditorInstance;
#else
	AppInstance = CreateAppInstance();
#endif
	FEngine::CreateEngine(AppInstance);

	GEngine->Initialize();
	
	int32 ExitCode = GEngine->Run();

	GEngine->ShutDown();

	return ExitCode;
}
