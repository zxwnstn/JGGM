#pragma once

#include "JGGM.h"

class JGGMDeviceContext;
class JGGMSwapChain;

namespace JGGMABackEnd
{
	class VirtualGPU
	{
	private:

	public:
		static void Dispatch();

		static void* GPUWorkThread;
	};

}


class JGGMAdapter
{
private:
	JGGMAdapter();

public:
	static JGGMAdapter* GetSinleAdapter() { return SingleAdapter; }

	JGGM_RESULT CreateDeviceContext(class JGGMDeviceContext** ppDeviceContext);
	JGGM_RESULT CreateSwapChain(JGGMSwapChain** ppSwapChain);

public:
	friend JGGM_RESULT JGGMCreateAdapter(JGGMAdapter** ppAdapter);

private:
	static JGGMAdapter* SingleAdapter; // Adapter singleton

	JGGMDeviceContext* SinleDeviceContext;
	JGGMSwapChain* SingleSwapChain;
};



