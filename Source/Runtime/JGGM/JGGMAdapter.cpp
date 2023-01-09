#include "JGGMPch.h"

#include "JGGMAdapter.h"
#include "JGGMDeviceContext.h"
#include "JGGMSwapChain.h"

JGGMAdapter* JGGMAdapter::SingleAdapter = nullptr;

JGGMAdapter::JGGMAdapter()
	: SinleDeviceContext(nullptr)
	, SingleSwapChain(nullptr)
{
}

JGGM_RESULT JGGMAdapter::CreateDeviceContext(JGGMDeviceContext** ppDeviceContext)
{
	if (SinleDeviceContext)
	{
		// LOG - Adapter can have only one DeviceContext instance
		return JGGM_RESULT::JGGM_INVALID_ACTION;
	}
	SinleDeviceContext = new JGGMDeviceContext;
	*ppDeviceContext = SinleDeviceContext;

	return JGGM_RESULT::JGGM_SUCCESS;
}

JGGM_RESULT JGGMAdapter::CreateSwapChain(JGGMSwapChain** ppSwapChain)
{
	if (SingleSwapChain)
	{
		// LOG - Adapter can have only one SwapChain instance
		return JGGM_RESULT::JGGM_INVALID_ACTION;
	}
	SingleSwapChain = new JGGMSwapChain;
	*ppSwapChain = SingleSwapChain;

	return JGGM_RESULT::JGGM_SUCCESS;
}

JGGM_RESULT JGGMCreateAdapter(JGGMAdapter** ppAdapter)
{
	if (JGGMAdapter::SingleAdapter)
	{
		// LOG - Can have only one instance
		return JGGM_RESULT::JGGM_INVALID_ACTION;
	}

	JGGMAdapter::SingleAdapter = new JGGMAdapter;
	*ppAdapter = JGGMAdapter::SingleAdapter;

	return JGGM_RESULT::JGGM_SUCCESS;
}
