#include "JGGMPch.h"

#include "ThreadManager.h"

SThreadManager SThreadManager::Inst;

SThreadManager::~SThreadManager()
{
}

const FString GetThreadName(EThreadType Type)
{
    switch (Type)
    {
    case EThreadType::Main:
        return FString(TEXT("MainThread"));
    case EThreadType::Rendering:
        return FString(TEXT("RenderingThread"));
    case EThreadType::RHI:
        return FString(TEXT("RHIThread"));
    case EThreadType::VirtualGPU:
        return FString(TEXT("GPUThread"));
    case EThreadType::Worker:
        return FString(TEXT("WorkerThread"));
    default:
		return FString();
    }
    return FString();
}

bool SThreadManager::Initialize()
{
    if (NamedThread[0] != nullptr)
    {   
        // log - already initialized
        return false;
    }
    
    TaskIDSource = 0;

    // Main Thread
    {
        FThread* MainThread = CreateThreadInstance();
        MainThread->ThreadID = ::GetCurrentThreadId();
        MainThread->Type = EThreadType::Main;
        MainThread->bIsRunning = true;
        MainThread->bLaunched = false;
        MainThread->bRequestedExit = false;
        MainThread->bTerminated = false;

        FInitialThreadTask Task(MainThread->ThreadID);
        Task.DoTask();

        NamedThread[0] = MainThread;
    }

    // Named Thread
    for (int32 i = EThreadType::Rendering; i <= EThreadType::VirtualGPU; ++i)
    {
		FThread* Thread = CreateThreadInstance();
		Thread->Initialize(static_cast<EThreadType>(i), i + ManagedThreadTypeCount, GetThreadName(static_cast<EThreadType>(i)));
        NamedThread[i] = Thread;
        Thread->Launch();
    }

    // Worker Thread
    WorkerThreadCount = 10; // TODO : Query CPUs Thread or could be force it
    for (int32 i = 0; i < WorkerThreadCount; ++i)
    {
        FThread* Thread = CreateThreadInstance();

        FStringStream Stream;
        Stream << GetThreadName(EThreadType::Worker) << i;

		Thread->Initialize(EThreadType::Worker, i + ManagedThreadTypeCount, Stream.str());
        WorkerThreadPool.insert({Thread->ThreadID, Thread});
		Thread->Launch();

        FQueuedTaskHandle TaskHandle = Thread->EnqueueTask(SThreadManager::Get().CreateTask<FInitialWorkerThreadTask>());
        TaskHandle.Wait();
        TaskHandle.Finish();
    }

    return true;
}

bool SThreadManager::ShutDown()
{
	for (int32 i = EThreadType::Rendering; i <= EThreadType::VirtualGPU; ++i)
	{
        FThread* Thread = NamedThread[i];
        Thread->Terminate(false);
        delete Thread;
	}

    for (auto It = WorkerThreadPool.begin(); It != WorkerThreadPool.end(); ++It)
    {
        FThread* Worker = It->second;
        Worker->Terminate(false);
        delete Worker;
    }
    WorkerThreadPool.clear();

    for (auto It = CustomThreadPool.begin(); It != CustomThreadPool.end(); ++It)
    {
        FThread* CustomThread = It->second;
        CustomThread->Terminate(false);
        delete CustomThread;
    }
    CustomThreadPool.clear();

    return false;
}

FThread* SThreadManager::CreateCustomThread(const FString& Name, FThreadMain* Main)
{
    FThread* Thread = CreateThreadInstance();
    Main->MyThread = Thread;

    Thread->Initialize(EThreadType::CustomThread, 0, Name);
    CustomThreadPool.insert({ Thread->ThreadID, Thread });
    Thread->ThreadMain = Main;
    Thread->Launch();

    return Thread;
}


FThread* SThreadManager::CreateThreadInstance()
{
    extern FThread* CreatePlatformThread();
    FThread* Thread = CreatePlatformThread();

    return Thread;
}

FThread* SThreadManager::GetMostFreeWorkerThread()
{
    uint32 LeastWorkCount = INT_MAX;
    FThread* MostFreeThread = nullptr;
	for (auto It : WorkerThreadPool)
	{
		FThread* Thread = It.second;
        if (Thread->TaskQueue.size() < LeastWorkCount)
        {
            LeastWorkCount = Thread->TaskQueue.size();
            MostFreeThread = Thread;
            if (LeastWorkCount == 0)
            {
                break;
            }
        }
	}

    return MostFreeThread;
}

FThread* SThreadManager::GetNamedThread(EThreadType Type)
{
    return NamedThread[Type];
}

std::vector<FThread*> SThreadManager::GetWorkerThreads()
{
    static std::vector<FThread*> Workers;
    if (Workers.empty())
    {
        for (const auto& It : WorkerThreadPool)
        {
            Workers.push_back(It.second);
        }
    }
    return Workers;
}

FThread* SThreadManager::GetThreadFromID(uint32 ThreadID)
{
    // Check Named thread first
	for (int32 i = EThreadType::Rendering; i <= EThreadType::VirtualGPU; ++i)
	{
		FThread* Thread = NamedThread[i];
        if (ThreadID == Thread->ThreadID)
        {
            return Thread;
        }
	}

    // Find WorkerThread
    auto WorkerFound = WorkerThreadPool.find(ThreadID);
    if (WorkerFound != WorkerThreadPool.end())
    {
        return WorkerFound->second;
    }

    // Find CustomThread
    auto CustomThreadFound = CustomThreadPool.find(ThreadID);
    if (CustomThreadFound != CustomThreadPool.end())
    {
        return CustomThreadFound->second;
    }

    return nullptr;
}
