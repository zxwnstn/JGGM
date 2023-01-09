#pragma once

class FThread;
class FEvent;

enum EThreadType
{
	Main,
	Render,
	// RHI, 
	GPU,
	Worker,

	ThreadTypeCount,
	Undefiend,
};

class FQueuedTaskHandle
{
public:
	FQueuedTaskHandle();

	void Wait();
	void Cancel();
	void Finish();
	
	bool IsValid();
	bool IsInflight();
	bool IsDone();

	template<typename TaskType>
	TaskType* GetTask()
	{
		TaskType* Task = SThreadManager::Get().GetTask<TaskType>(*this);
		return Task;
	}

private:
	FThread* GetOwnerThread();

	uint64 TaskID;
	FThread* OwnerThread;
	uint32 OwnerThreadID;

	bool bDone;
	bool bValid;

	friend class FThread;
	friend class SThreadManager;
};

class FThreadTask
{
public:
	virtual void DoTask() = 0;

protected:
	uint64 TaskID;
	FQueuedTaskHandle Handle;
	FEvent* TaskEvent;

	friend class FThread;
	friend class FNormalQueuedThreadMain;
	friend class SThreadManager;
};

class FInitialThreadTask : public FThreadTask
{
public:
	FInitialThreadTask(uint32 InThreadID)
		: ThreadID(InThreadID)
	{}
	virtual void DoTask();
	
	uint32 ThreadID;
};


class FThreadMain
{
public:
	FThreadMain(FThread* InThread);
	virtual ~FThreadMain();

	virtual int32 operator()() = 0;

	FThread* MyThread;
};


class FThread
{
protected:
	FThread();
	virtual ~FThread();

	// Interface's for ThreadManager
protected:
	virtual void Initialize(EThreadType InType, uint32 InThreadID);
	virtual void Launch();
	void Terminate(bool bForce);
	int32 Run();

	// FThread inner operation
private:
	FThreadTask* DequeueTask();

	void Sleep();
	void WakeUp();

public:
	inline bool IsRequestedExit() { return bRequestedExit; }
	inline bool IsTerminated() { return bTerminated; }
	inline bool IsLaunched() { return bLaunched; } 

	bool IsRunningTask();
	void WaitTask(FQueuedTaskHandle& Task);
	void FlushTasks();

	FQueuedTaskHandle QueueTask(FThreadTask* Task);
	void CancelTask(FQueuedTaskHandle& TaskHandle);

	bool QueryTaskValid(const FQueuedTaskHandle& TaskHandle);
	bool QueryTaskInflight(const FQueuedTaskHandle& TaskHandle);

protected:
	EThreadType Type;
	uint32 ThreadID;

	bool bLaunched;
	bool bTerminated;
	bool bRequestedExit;
	bool bIsRunning;

	std::vector<FThreadTask*> TaskQueue;
	std::mutex TaskQueueMutex;

	uint64 LastCompletedTaskID;
	uint64 InflightTaskID;

	FThreadMain* ThreadMain;
	FEvent* RunningEvent;

	friend class SThreadManager;
	friend class FNormalQueuedThreadMain;
};

uint32 GetCurrentThreadID();

