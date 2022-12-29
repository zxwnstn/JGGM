#include "JGGMAdapter.h"


int main()
{
	JGGMAdapter* Adapter = nullptr;
	JGGM_ASSERT(JGGMCreateAdapter(&Adapter));

	JGGMDeviceContext* DeviceContext = nullptr;
	JGGM_ASSERT(Adapter->CreateDeviceContext(&DeviceContext));

}