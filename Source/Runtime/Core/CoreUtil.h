#pragma once

uint32 GetCurrentThreadID();
bool IsInMainThread();
bool IsInRenderingThread();
bool IsInGPUThread();
bool IsInRHIThread();
bool IsInWorkerThread();
