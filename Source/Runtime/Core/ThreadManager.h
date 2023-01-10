#pragma once

#include "Thread.h"
#include "Event.h"


const FString GetThreadName(EThreadType Type);

class SThreadManager
{
private:
	SThreadManager() = default;
	~SThreadManager();

public:
	static SThreadManager& Get() { return Inst; }
	bool Initialize();
	bool ShutDown();

	FThread* CreateCustomThread(const FString& Name, FThreadMain* Main);

private:
	FThread* CreateThreadInstance();
	
public:
	// Thread Management
	FThread* GetNamedThread(EThreadType Type);
	std::vector<FThread*> GetWorkerThreads();
	FThread* GetThreadFromID(uint32 ThreadID);
	FThread* GetMostFreeWorkerThread();

	// Task Management
	template<typename TaskType, typename... Args>
	FThreadTask* CreateTask(Args&&... args)
	{
		TaskIDSourceMutex.lock();
		int64 TaskID = ++TaskIDSource;
		TaskIDSourceMutex.unlock();

		FThreadTask* Task = new TaskType(std::forward<Args>(args)...);
		Task->TaskID = TaskID;
		Task->TaskEvent = FEvent::GetEvent();

		TaskStorageMutex.lock();
		TaskStorage.insert({ TaskID, Task});
		TaskStorageMutex.unlock();

		return Task;
	}

	template<typename TaskType> 
	TaskType* GetTask(const FQueuedTaskHandle& TaskHandle)
	{
		TaskStorageMutex.lock();
		auto Found = TaskStorage.find(TaskHandle.TaskID);
		if (Found != TaskStorage.end())
		{
			TaskType* Task = (TaskType*)(Found->second);
			TaskStorageMutex.unlock();
			return Task;
		}
		TaskStorageMutex.unlock();
		return nullptr;
	}

	void FinishTask(const FQueuedTaskHandle& TaskHandle)
	{
		TaskStorageMutex.lock();
		auto Found = TaskStorage.find(TaskHandle.TaskID);
		if (Found != TaskStorage.end())
		{
			FThreadTask* Task = Found->second;
			delete Task;
			TaskStorage.erase(Found);
		}
		else
		{
			// log - aleady finished task
		}
		TaskStorageMutex.unlock();
	}

public:
	template<typename TaskType, typename... Args>
	friend FQueuedTaskHandle EnqueueThreadTask(EThreadType ThreadType, Args&&... args)
	{
		FQueuedTaskHandle TaskHandle;
		SThreadManager& ThreadManager = SThreadManager::Get();
		FThread* Thread = nullptr;
		if (ThreadType == EThreadType::Main)
		{
			// log - 
			return TaskHandle;
		}
		else if (ThreadType < EThreadType::Worker)
		{
			Thread = ThreadManager.GetNamedThread(EThreadType::Rendering);
		}
		else if(ThreadType == EThreadType::Worker)
		{
			Thread = ThreadManager.GetMostFreeWorkerThread();
		}
		else if (ThreadType == EThreadType::CustomThread)
		{
			// ensure
			return TaskHandle;
		}
		else
		{
			// log - 
			return TaskHandle;
		}

		FThreadTask* Task = ThreadManager.CreateTask<TaskType>(std::forward<Args>(args)...);

		TaskHandle = Thread->EnqueueTask(Task);

		return TaskHandle;
	}


private:
	static SThreadManager Inst;

	uint32 WorkerThreadCount;
	std::unordered_map<uint32, FThread*> WorkerThreadPool;
	std::unordered_map<uint32, FThread*> CustomThreadPool;
	FThread* NamedThread[ManagedThreadTypeCount];

	uint64 TaskIDSource;
	std::mutex TaskIDSourceMutex;
	std::unordered_map<uint64, FThreadTask*> TaskStorage;
	std::mutex TaskStorageMutex;
};

