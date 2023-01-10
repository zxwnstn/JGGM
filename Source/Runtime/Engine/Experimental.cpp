#include "JGGMPch.h"

#include "Core/Thread.h"
#include "Core/ThreadManager.h"


class FAddTask : public FThreadTask
{
public:
	FAddTask()
	{}

	~FAddTask()
	{}

	virtual void DoTask()
	{
		Result = 0;
		for (int64 i = 0; i <= 200000; ++i)
		{
			Result = i;
		}
	}

public:
	int64 Result;
};

class FThreadedTask : public FThreadTask
{
public:
	FThreadedTask(int32 InUseThreadCount)
		: UseThreadCount(InUseThreadCount)
		, Total(0)
	{}

	virtual void DoTask()
	{
		int32 Result = 0;
		for (int32 i = 0; i < UseThreadCount; ++i)
		{
			TaskHandles.push_back(EnqueueThreadTask<FAddTask>(EThreadType::Worker));
		}

		for (auto& Handle : TaskHandles)
		{
			if (!Handle.IsDone())
			{
				Handle.Wait();
			}
			Total += Handle.GetTask<FAddTask>()->Result;
			Handle.Finish();
		}
	}

public:
	int32 UseThreadCount;
	int64 Total;
	std::vector<FQueuedTaskHandle> TaskHandles;
};


void ThreadExperimentalWork()
{
	static FQueuedTaskHandle EditorJobHandle;
	static int Count = 0;
	static int Max = 10;

	if (!EditorJobHandle.IsValid() && Count < Max)
	{
		EditorJobHandle = EnqueueThreadTask<FThreadedTask>(EThreadType::Rendering, 30);
		++Count;
	}
	else
	{
		if (EditorJobHandle.IsDone())
		{
			FThreadedTask* Task = EditorJobHandle.GetTask<FThreadedTask>();
			// enusre(Task)
			int64 Result = Task->Total;
			std::cout << Result;
			EditorJobHandle.Finish();
		}
		else
		{
			if (EditorJobHandle.IsValid())
			{
				int mode = 1;
				if (mode == 0)
				{
					EditorJobHandle.Wait();
				}
			}
		}
	}
}
