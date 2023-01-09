#pragma once

#include "Thread.h"
#include "Event.h"

class SThreadManager
{
private:
	SThreadManager() = default;
	~SThreadManager();

	bool Initialize();

public:
	static SThreadManager& Get() { return Inst; }

private:
	FThread* CreateThreadInstance();

public:
	// Thread Management
	FThread* GetThreadFromID(uint32 ThreadID);
	FThread* GetMostFreeWorkerThread();

	// Task Management
	template<typename TaskType, typename... Args>
	FThreadTask* CreateTask(Args&&... args)
	{
		TaskIDSourceMutex.lock();
		int64 TaskID = TaskIDSource++;
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
			TaskStorage.erase(Found);
		}
		else
		{
			// log - aleady finished task
		}
		TaskStorageMutex.unlock();
	}


private:
	static SThreadManager Inst;

	uint32 ThreadIDSource;
	std::unordered_map<uint32, FThread*> WorkerThreadPool;
	FThread* NamedThread[ThreadTypeCount];

	uint64 TaskIDSource;
	std::mutex TaskIDSourceMutex;
	std::unordered_map<uint64, FThreadTask*> TaskStorage;
	std::mutex TaskStorageMutex;
};

