
#include "Engine/AppInstance.h"
#include <iostream>

class FSandboxInstance
	: public IAppInstance
{
public:
	void Initialize() override
	{
		IAppInstance::Initialize();
		std::cout << "Sandbox Instance Initialized!";
	}

	void Tick() override
	{
	}
};

IAppInstance* CreateAppInstance()
{
	return new FSandboxInstance;
}