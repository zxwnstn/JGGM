#include "JGGMPch.h"

#if _WIN32
#include "../Event.h"

class FWindowsEvent : public FEvent
{
public:
	
	FWindowsEvent()
	{
		EventWindowsHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	~FWindowsEvent()
	{
		CloseHandle(EventWindowsHandle);
	}

	virtual void Wait() override
	{
		WaitForSingleObject(EventWindowsHandle, INFINITE);
	}

	virtual void Signal() override
	{
		EventMutex.lock();
		SetEvent(EventWindowsHandle);
		EventMutex.unlock();
	}

	virtual void Reset() override
	{
		EventMutex.lock();
		ResetEvent(EventWindowsHandle);
		EventMutex.unlock();
	}

	HANDLE EventWindowsHandle;
};

FEvent* CreatePlatformEvent()
{
	return new FWindowsEvent;
}

#endif