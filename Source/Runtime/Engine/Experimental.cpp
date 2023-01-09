#include "JGGMPch.h"

#include "Core/Thread.h"
#include "Core/ThreadManager.h"

class FAddTask : public FThreadTask
{
public:
	FAddTask()
	{}

	FAddTask(int32 InMultiplier)
		: Multiplier(InMultiplier)
	{}

	virtual void DoTask()
	{
		Result = 0;
		for (uint32 i = 0; i < 200000; ++i)
		{
			Result += i;
		}
	}

public:
	int32 Multiplier;
	int32 Result;
};


void ThreadExperimentalWork()
{
	static FQueuedTaskHandle EditorJobHandle;

	FThread* WorkerThread = SThreadManager::Get().GetMostFreeWorkerThread();
	if (!EditorJobHandle.IsValid())
	{
		EditorJobHandle = WorkerThread->QueueTask(SThreadManager::Get().CreateTask<FAddTask>(3));
	}
	else
	{
		if (EditorJobHandle.IsDone())
		{
			FAddTask* Task = EditorJobHandle.GetTask<FAddTask>();
			// enusre(Task)
			int32 Result = Task->Result;
			EditorJobHandle.Finish();
		}
		else
		{
			int mode = 0;
			if (mode == 0)
			{
				EditorJobHandle.Wait();
			}
		}
	}
}
