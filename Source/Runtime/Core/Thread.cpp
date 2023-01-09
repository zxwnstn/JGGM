#include "JGGMPch.h"

#include "Event.h"
#include "Thread.h"
#include "ThreadManager.h"

thread_local uint32 TLSThreadID;

FQueuedTaskHandle::FQueuedTaskHandle()
{
}

void FQueuedTaskHandle::Wait()
{
	if (IsValid())
	{
		OwnerThread->WaitTask(*this);
	}
}

void FQueuedTaskHandle::Cancel()
{
	if (IsValid())
	{
		if (IsInflight())
		{
			// log - Task is aleady inflight, we can't do anything now
		}
		else
		{
			OwnerThread->CancelTask(*this);
		}
	}
}

void FQueuedTaskHandle::Finish()
{
	SThreadManager::Get().FinishTask(*this);

	bValid = false;
	OwnerThread = nullptr;
	this->bDone = true;
	this->OwnerThreadID = INVALID_ID_32;
	this->TaskID = INVALID_ID_64;
}

bool FQueuedTaskHandle::IsValid()
{
	return bValid;
}

bool FQueuedTaskHandle::IsInflight()
{
	if (IsValid())
	{
		return OwnerThread->QueryTaskInflight(*this);
	}
	return false;
}

bool FQueuedTaskHandle::IsDone()
{
	return bDone;
}

FThread* FQueuedTaskHandle::GetOwnerThread()
{
	if (!OwnerThread)
	{
		OwnerThread = SThreadManager::Get().GetThreadFromID(OwnerThreadID);
	}
	return OwnerThread;
}

FThread::FThread()
	: Type(EThreadType::Undefiend)
	, bLaunched(false)
	, bIsRunning(false)
	, bRequestedExit(false)
	, bTerminated(false)
	, InflightTaskID(INVALID_ID_64)
	, LastCompletedTaskID(INVALID_ID_64)
	, ThreadID(INVALID_ID_32)
	, ThreadMain(nullptr)
	, RunningEvent(nullptr)
{
}

FThread::~FThread()
{
	FEvent::ReleaseEvent(RunningEvent);
}

FThreadMain::FThreadMain(FThread* InThread)
	: MyThread(InThread)
{
}

FThreadMain::~FThreadMain()
{
}


class FNormalQueuedThreadMain : public FThreadMain
{
public:
	FNormalQueuedThreadMain(FThread* InThread)
		: FThreadMain(InThread)
	{}

	int32 operator()()
	{
		while (!MyThread->IsRequestedExit())
		{
			if (FThreadTask* Task = MyThread->DequeueTask())
			{
				Task->DoTask();
			}
			else
			{
				MyThread->Sleep();
			}
		}

		return 0;
	}
};

void FThread::Initialize(EThreadType InType, uint32 InThreadID)
{
	Type = InType;
	ThreadID = InThreadID;
	RunningEvent = FEvent::GetEvent();
	switch (InType)
	{
	case Main:
		break;
	case Render:
	case GPU:
	case Worker:
		ThreadMain = new FNormalQueuedThreadMain(this);
		break;
	default :
		break;
	}
}

void FThread::Launch()
{
	if (!bLaunched)
	{
		bLaunched = true;
	}
	else
	{
		// log - This Thread is aleady launched
	}
}

void FThread::Terminate(bool bForce)
{
	bRequestedExit = true;
	FlushTasks();

	bTerminated = true;
	bLaunched = false;
}

int32 FThread::Run()
{
	return (*ThreadMain)();
}

FQueuedTaskHandle FThread::QueueTask(FThreadTask* Task)
{
	FQueuedTaskHandle TaskHandle;
	TaskHandle.OwnerThreadID = ThreadID;
	TaskHandle.TaskID = Task->TaskID;
	TaskHandle.bDone = false;
	TaskHandle.bValid = true;

	if (GetCurrentThreadID() == ThreadID)
	{
		TaskQueue.push_back(Task);
	}
	else
	{
		TaskQueueMutex.lock();
		TaskQueue.push_back(Task);
		TaskQueueMutex.unlock();
	}

	FThread* Thread = SThreadManager::Get().GetThreadFromID(ThreadID);
	if (!Thread->IsRunningTask())
	{
		Thread->WakeUp();
	}

	return TaskHandle;
}

void FThread::WaitTask(FQueuedTaskHandle& TaskHandle)
{

	
}

void FThread::FlushTasks()
{
}

void FThread::CancelTask(FQueuedTaskHandle& TaskHandle)
{
	if (QueryTaskValid(TaskHandle))
	{
		if (QueryTaskInflight(TaskHandle))
		{
			// Nothing todo just wait
			WaitTask(TaskHandle);
		}
		else
		{
			TaskQueueMutex.lock();
			for (auto It = TaskQueue.begin(); It != TaskQueue.end(); ++It)
			{
				if ((*It)->TaskID == TaskHandle.TaskID)
				{
					TaskQueue.erase(It);
				}
			}
			TaskQueueMutex.unlock();
		}
	}
}

bool FThread::QueryTaskValid(const FQueuedTaskHandle& TaskHandle)
{
	return !TaskHandle.bDone && TaskHandle.TaskID > LastCompletedTaskID;
}

bool FThread::QueryTaskInflight(const FQueuedTaskHandle& TaskHandle)
{
	return TaskHandle.TaskID == InflightTaskID;
}

FThreadTask* FThread::DequeueTask()
{
	FThreadTask* Task;
	TaskQueueMutex.lock();
	if (!TaskQueue.empty())
	{
		bIsRunning = true;
		Task = *TaskQueue.begin();
		TaskQueue.erase(TaskQueue.begin());
	}
	else
	{
		bIsRunning = false;
		return nullptr;
	}
	InflightTaskID = Task->TaskID;
	TaskQueueMutex.unlock();
	return Task;
}

void FThread::Sleep()
{
	RunningEvent->Reset();
	RunningEvent->Wait();
}

void FThread::WakeUp()
{
	RunningEvent->Signal();
}


bool FThread::IsRunningTask()
{
	return false;
}


uint32 GetCurrentThreadID()
{
	return TLSThreadID;
}

void FInitialThreadTask::DoTask()
{
	TLSThreadID = this->ThreadID;
}

