#include "JGGMPch.h"

#include "Event.h"
#include "Thread.h"
#include "ThreadManager.h"

thread_local uint32 TLSThreadID = 0;
thread_local bool TLSIsWorkerThread = false;

FQueuedTaskHandle::FQueuedTaskHandle()
	: TaskID(INVALID_ID_64)
	, OwnerThread(nullptr)
	, OwnerThreadID(INVALID_ID_32)
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
		OwnerThread->CancelTask(*this);
		Finish();
	}
}

void FQueuedTaskHandle::Finish()
{
	SThreadManager::Get().FinishTask(*this);

	OwnerThread = nullptr;
	OwnerThreadID = INVALID_ID_32;
	TaskID = INVALID_ID_64;
}

bool FQueuedTaskHandle::IsValid()
{
	return GetOwnerThread()->QueryTaskValid(*this);
}

bool FQueuedTaskHandle::IsInflight()
{
	return GetOwnerThread()->QueryTaskInflight(*this);
}

bool FQueuedTaskHandle::IsDone()
{
	return GetOwnerThread()->QueryTaskIsDone(*this);
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
	, bIsRunningTask(false)
	, bRequestedExit(false)
	, bTerminated(false)
	, bIsSuspended(false)
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

FThreadMain::FThreadMain()
	: MyThread(nullptr)
{
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
			while (FThreadTask* Task = MyThread->DequeueTask())
			{
				if (Task->bIsDetached)
				{
					Task->DoTask();
					Task->Handle.Finish();
				}
				else
				{
					MyThread->InflightTaskID = Task->TaskID;
					Task->DoTask();
					MyThread->LastCompletedTaskID = MyThread->InflightTaskID;
					Task->TaskEvent->Signal();
				}
			}
			if (!MyThread->IsRequestedExit())
			{
				MyThread->bIsRunningTask = false;
				MyThread->Sleep();
			}
		}

		return 0;
	}
};

void FThread::Initialize(EThreadType InType, uint32 InThreadID, const FString& ThreadName)
{
	Type = InType;
	ThreadID = InThreadID;
	RunningEvent = FEvent::GetEvent();
	ExitEvent = FEvent::GetEvent();
	ExitEvent->Reset();
	ExitCode = 0;
	LastCompletedTaskID = 0;
	InflightTaskID = 0;
	bIsRunningTask = false;
	bIsSuspended = true;

	switch (InType)
	{
	case Main:
		break;
	case Rendering:
	case RHI:
	case VirtualGPU:
	case Worker:
		ThreadMain = new FNormalQueuedThreadMain(this);
		break;
	default :
		break;
	}
}

bool FThread::Launch()
{
	if (!bLaunched)
	{
		bLaunched = true;
		FQueuedTaskHandle InitialTaskHandle = EnqueueTask(SThreadManager::Get().CreateTask<FInitialThreadTask>(ThreadID));
		InitialTaskHandle.Wait();
		InitialTaskHandle.Finish();
		return true;
	}
	else
	{
		// log - This Thread is aleady launched
	}
	return false;
}

bool FThread::Terminate(bool bForce)
{
	if (!bTerminated && IsInMainThread())
	{
		if (!bForce)
		{
			bRequestedExit = true;
			FlushTasks();
		}
		ExitEvent->Wait();

		bTerminated = true;
		bLaunched = false;

		return true;
	}
	return false;
}

bool FThread::Resume()
{
	if (bIsSuspended && IsInMainThread())
	{
		bIsSuspended = false;
		return true;
	}
	return false;
}

bool FThread::Suspend()
{
	if (!bIsSuspended && IsInMainThread())
	{
		bIsSuspended = true;
		return true;
	}
	bIsSuspended = false;
}

int32 FThread::Run()
{
	ExitCode = (*ThreadMain)();
	ExitEvent->Signal();

	return ExitCode;
}

FQueuedTaskHandle FThread::EnqueueTask(FThreadTask* Task)
{
	FQueuedTaskHandle TaskHandle;
	TaskHandle.OwnerThreadID = ThreadID;
	TaskHandle.TaskID = Task->TaskID;

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

	if (!IsRunningTask())
	{
		WakeUp();
	}

	return TaskHandle;
}

void FThread::EnqueueDetachedTask(FThreadTask* Task)
{
	Task->SetDetached(true);
	EnqueueTask(Task);
}

void FThread::WaitTask(FQueuedTaskHandle& TaskHandle)
{
	FThreadTask* Task = SThreadManager::Get().GetTask<FThreadTask>(TaskHandle);
	WaitTask(Task);
}

void FThread::WaitTask(FThreadTask* Task)
{
	Task->TaskEvent->Wait();
}

void FThread::FlushTasks()
{
	TaskQueueMutex.lock();
	FThreadTask* LastQueuedTask = nullptr;
	if (TaskQueue.size())
	{
		LastQueuedTask = TaskQueue.back();
	}
	else if(InflightTaskID != INVALID_ID_64)
	{
		LastQueuedTask = SThreadManager::Get().GetTask<FThreadTask>(InflightTaskID);
	}
	TaskQueueMutex.unlock();

	if (LastQueuedTask)
	{
		WaitTask(LastQueuedTask);
		LastQueuedTask->Handle.Finish();
	}
}

void FThread::CancelTask(FQueuedTaskHandle& TaskHandle)
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

bool FThread::QueryTaskValid(const FQueuedTaskHandle& TaskHandle)
{
	return (TaskHandle.TaskID != INVALID_ID_64) && (TaskHandle.TaskID >= LastCompletedTaskID);
}

bool FThread::QueryTaskIsDone(const FQueuedTaskHandle& TaskHandle)
{
	return (TaskHandle.TaskID != INVALID_ID_64) && (TaskHandle.TaskID <= LastCompletedTaskID);
}

bool FThread::QueryTaskInflight(const FQueuedTaskHandle& TaskHandle)
{
	return (TaskHandle.TaskID != INVALID_ID_64) && (TaskHandle.TaskID == InflightTaskID);
}

FThreadTask* FThread::DequeueTask()
{
	FThreadTask* Task = nullptr;
	if (!TaskQueue.empty())
	{
		bIsRunningTask = true;
		TaskQueueMutex.lock();
		Task = *TaskQueue.begin();
		TaskQueue.erase(TaskQueue.begin());
		TaskQueueMutex.unlock();
	}
	else
	{
		bIsRunningTask = false;
	}
	InflightTaskID = Task ? Task->TaskID : INVALID_ID_64;
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
	return bIsRunningTask;
}

uint32 GetCurrentThreadID()
{
	return TLSThreadID;
}

bool IsInMainThread()
{
	return GetCurrentThreadID() == SThreadManager::Get().GetNamedThread(EThreadType::Main)->GetThreadID();
}

bool IsInRenderingThread()
{
	return GetCurrentThreadID() == SThreadManager::Get().GetNamedThread(EThreadType::Rendering)->GetThreadID();
}

bool IsInGPUThread()
{
	return GetCurrentThreadID() == SThreadManager::Get().GetNamedThread(EThreadType::VirtualGPU)->GetThreadID();
}

bool IsInRHIThread()
{
	return GetCurrentThreadID() == SThreadManager::Get().GetNamedThread(EThreadType::RHI)->GetThreadID();
}

bool IsInWorkerThread()
{
	return TLSIsWorkerThread;
}

void FInitialThreadTask::DoTask()
{
	TLSThreadID = this->ThreadID;
}

void FInitialWorkerThreadTask::DoTask()
{
	TLSIsWorkerThread = true;
}
