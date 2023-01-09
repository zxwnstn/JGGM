#include "JGGMPch.h"

#include "Engine/Engine.h"
#include "Engine/AppInstance.h"

int JGGMMain()
{
	IAppInstance* AppInstance;
#if JGGM_EDITOR
	AppInstance = new FEditorInstance;
#else
	AppInstance = CreateAppInstance();
#endif
	FEngine::CreateEngine(AppInstance);

	GEngine->Initialize();
	
	int ExitCode = GEngine->Run();

	return ExitCode;
}

int main(int argc, char** argv)
{
#if _WIN32
#endif
	int ExitCode = 0;
	try {
		int ExitCode = JGGMMain();
	}
	catch(...){

	}

	return ExitCode;
}

