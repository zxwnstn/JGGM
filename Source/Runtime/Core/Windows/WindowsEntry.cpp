#include "JGGMPch.h"

#include "Core/Timer.h"

int main(int argc, char** argv)
{
#if _WIN32
#endif
	FTimer::Initialize();

	int ExitCode = 0;
	try {
		extern int32 JGGMMain();
		int ExitCode = JGGMMain();
	}
	catch (...) {

	}

	return ExitCode;
}

