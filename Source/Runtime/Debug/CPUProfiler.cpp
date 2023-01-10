#include "JGGMPch.h"

#include "CPUProfiler.h"
#include "Core/Thread.h"
#include "Core/ThreadManager.h"

SCPUProfiler SCPUProfiler::Inst;

class FProfileTask : public FThreadTask
{
public:
	FProfileTask(std::vector<FProfileData>&& BufferedDatas)
		: Datas(std::move(BufferedDatas))
	{}

	std::vector<FProfileData> Datas;
};

class FProfileThreadMain : public FThreadMain
{
public:
	FProfileThreadMain() {}

	void WriteJson(const FProfileData& Data)
	{

	}

	virtual int32 operator()()
	{
		while (!MyThread->IsRequestedExit())
		{
			if (FProfileTask* Task = (FProfileTask*)MyThread->DequeueTask())
			{
				for (auto& Data : Task->Datas)
				{
					WriteJson(Data);
				}
				delete Task;
			}
			else
			{
				MyThread->bIsRunning = false;
				MyThread->Sleep();
			}
		}
		return 0;
	}

private:
	FStringStream Stream;
};



void SCPUProfiler::Initialize()
{
	if (bUseThread)
	{
		ProfileThread = SThreadManager::Get().CreateCustomThread(TEXT("CPUProfile"), new FProfileThreadMain);
	}
	else
	{
		ProfileMain = new FProfileThreadMain;
	}

}

void SCPUProfiler::ShutDown()
{
	if (!bUseThread)
	{
		delete ProfileMain;
	}
}

void SCPUProfiler::EnqueueEvent(FProfileData&& Data)
{
	DataBufferMutex.lock();
	if (bUseThread)
	{
		DataBuffer.emplace_back(std::move(Data));
		if (DataBuffer.size() >= MaxBufferingDataCount)
		{
			ProfileThread->EnqueueDetachedTask(new FProfileTask(std::move(DataBuffer)));
			DataBuffer.clear();
		}
	}
	else
	{
		ProfileMain->WriteJson(Data);
	}
	DataBufferMutex.unlock();
}
