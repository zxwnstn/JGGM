#include "AppInstance.h"

void IAppInstance::Initialize()
{
}

void IAppInstance::Tick()
{
}

#if JGGM_EDITOR
void FEditorInstance::Initialize()
{
}

void FEditorInstance::Tick()
{
}
#endif