#pragma once

#include "Event.h"


using FMutex = std::mutex;

class ScopedLock
{
public:
	ScopedLock()
	{
		Mutex.lock();
	}
	~ScopedLock()
	{
		Mutex.unlock();
	}

	FMutex Mutex;
};


