#include "JGGMPch.h"

#if _WIN32
#include "Core/Thread.h"
#include "Core/Event.h"

class FWindowsThread : public FThread
{
private:
	FWindowsThread();
	~FWindowsThread();

	virtual void Initialize(EThreadType InType, uint32 InThreadID, const FString& ThreadName = FString()) override;
	virtual bool Launch() override;

	virtual bool Terminate(bool bForce) override;
	virtual bool Resume() override;
	virtual bool Suspend() override;


	static ::DWORD _stdcall ThreadMain(LPVOID Thread)
	{
		return ((FWindowsThread*)Thread)->Run();
	}

private:
	HANDLE WindowsThreadHandle;

	friend FThread* CreatePlatformThread();

	friend class SThreadManager;
};

FWindowsThread::FWindowsThread()
{
}

FWindowsThread::~FWindowsThread()
{
	::CloseHandle(WindowsThreadHandle);
}

void FWindowsThread::Initialize(EThreadType InType, uint32 InThreadID, const FString& ThreadName)
{
	FThread::Initialize(InType, ThreadID);
	WindowsThreadHandle = CreateThread(NULL, 0, ThreadMain, this, CREATE_SUSPENDED, (DWORD*)&ThreadID);
	if (!ThreadName.empty())
	{
		::SetThreadDescription(WindowsThreadHandle, ThreadName.data());
	}
}

bool FWindowsThread::Launch()
{
	Resume();
	return FThread::Launch();
}

bool FWindowsThread::Terminate(bool bForce)
{
	if (FThread::Terminate(bForce))
	{
		::CloseHandle(WindowsThreadHandle);
		return true;
	}
	return false;
}

bool FWindowsThread::Resume()
{
	if (FThread::Resume())
	{
		::ResumeThread(WindowsThreadHandle);
		return true;
	}
	return false;
}

bool FWindowsThread::Suspend()
{
	if (FThread::Suspend())
	{
		::SuspendThread(WindowsThreadHandle);
		return true;
	}
	return false;
}

FThread* CreatePlatformThread()
{
	FThread* WindowsThread = new FWindowsThread;
	return WindowsThread;
}

#endif
