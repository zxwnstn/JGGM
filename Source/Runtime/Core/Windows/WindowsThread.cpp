#include "JGGMPch.h"

#if _WIN32
#include "../Thread.h"


class FWindowsThread : public FThread
{
private:
	FWindowsThread();
	~FWindowsThread();

	virtual void Initialize(EThreadType InType, uint32 InThreadID) override;
	virtual void Launch() override;

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
	CloseHandle(WindowsThreadHandle);
}

void FWindowsThread::Initialize(EThreadType InType, uint32 InThreadID)
{
	WindowsThreadHandle = CreateThread(NULL, 0, ThreadMain, this, CREATE_SUSPENDED, (DWORD*)&ThreadID);
	FThread::Initialize(InType, ThreadID);
}

void FWindowsThread::Launch()
{
	FThread::Launch();
	ResumeThread(WindowsThreadHandle);
}

FThread* CreatePlatformThread()
{
	FThread* WindowsThread = new FWindowsThread;
	return WindowsThread;
}

#endif
