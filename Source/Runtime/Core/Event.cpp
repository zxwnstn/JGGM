#include "JGGMPch.h"

#include "Event.h"

std::vector<FEvent*> GEventPool;

#define DEFAULT_EVENT_POOL_SIZE 16

FEvent* FEvent::GetEvent()
{
	FEvent* Event = nullptr;
	if (GEventPool.empty())
	{
		GEventPool.reserve(DEFAULT_EVENT_POOL_SIZE);
		for (int32 i = 0; i < DEFAULT_EVENT_POOL_SIZE; ++i)
		{
			extern FEvent* CreatePlatformEvent(); 
			GEventPool.push_back(CreatePlatformEvent());
		}
	}

	for (int32 i = 0; i < GEventPool.size(); ++i)
	{
		if (!GEventPool[i]->bUsed)
		{
			Event = GEventPool[i];
		}
	}
	
	if (Event == nullptr)
	{
		int32 CurrentSize = GEventPool.size();
		int32 ReserveSize = CurrentSize * 2;
		GEventPool.reserve(ReserveSize);

		for (int32 i = 0; i < CurrentSize; ++i)
		{
			extern FEvent* CreatePlatformEvent();
			GEventPool.push_back(CreatePlatformEvent());
		}
		Event = GEventPool[CurrentSize];
	}
	Event->bUsed = true;

	return Event;
}

void FEvent::ReleaseEvent(FEvent* Event)
{
	Event->bUsed = false;
}

void FEvent::ShoutDownEventPool()
{
	for (int32 i = 0; i < GEventPool.size(); ++i)
	{
		//ensue(!GEventPool[i]->bUsed);
		delete GEventPool[i];
	}
}
