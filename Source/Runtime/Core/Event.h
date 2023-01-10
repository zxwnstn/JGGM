#pragma once

class FEvent
{
public:
	static FEvent* GetEvent();
	static void ReleaseEvent(FEvent* Event);

	static void ShoutDownEventPool();

public:
	FEvent()
		: bUsed(false)
	{}
	virtual ~FEvent() {};

	virtual void Wait() = 0;
	virtual void Signal() = 0;
	virtual void Reset() = 0;

protected:
	bool bUsed;
	std::mutex EventMutex;
};
