#include "JGGMPch.h"

#include "ThreadManager.h"

SThreadManager SThreadManager::Inst;

SThreadManager::~SThreadManager()
{
}

bool SThreadManager::Initialize()
{
    ThreadIDSource = 0;


    uint32 WorkerThreadCount = 10; // TODO : Query CPUs Thread or could be force it

    for (int32 i = 0; i < WorkerThreadCount; ++i)
    {
        FThread* Thread = CreateThreadInstance();
		Thread->Initialize(EThreadType::Worker, i + ThreadTypeCount);
    }

    return true;
}

FThread* SThreadManager::CreateThreadInstance()
{
    extern FThread* CreatePlatformThread();
    FThread* Thread = CreatePlatformThread();

    return Thread;
}

FThread* SThreadManager::GetMostFreeWorkerThread()
{
    return nullptr;
}

FThread* SThreadManager::GetThreadFromID(uint32 ThreadID)
{
    return nullptr;
}
