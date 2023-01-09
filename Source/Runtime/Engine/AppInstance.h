#pragma once

class IAppInstance
{
public:
	virtual void Initialize();
	virtual void Tick(float DeltaTime);
};

#if JGGM_EDITOR

class FEditorInstance
	: public IAppInstance
{
public:
	virtual void Initialize() override;
	virtual void Tick(float DeltaTime) override;
};

#else

extern IAppInstance* CreateAppInstance();

#endif