#pragma once

class IAppInstance;

class FEngine
{
private:
	FEngine(IAppInstance* AppInst);

public:
	void Initialize();
	void ShutDown();

	int32 Run();
	int32 Tick();

	static FEngine* CreateEngine(IAppInstance* AppInst);

private:
	IAppInstance* AppInst;
	bool bRequestedExit;
};

extern FEngine* GEngine;