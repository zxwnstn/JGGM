#pragma once

#include "JGGM.h"

class JGGMDeviceContext 
{
private:
	JGGMDeviceContext();


public:

	// Creator
	void CreateVertexStream();
	void CreateSissorRect();
	void CreateTexture();

	// Setter
	void SetVertexStream();
	void SetVertexShader();
	void SetRasterizerState();
	void SetPixelShader();
	void SetSissorRect();
	void SetRenderTarget();
	void SetShaderParam();
	void SetViewport();

private:

	struct InnerStateCache
	{
		void* VertexStream;
		void* VertexShader;
		void* VertexShaderParams;
		void* RasterizerState;
		void* PixelShader;
		void* PixelShaderParams;
		void* SissorRect;
		void* Viewport;
		void* RenderTarget;

	} StateCache;



	friend class JGGMAdapter;
};
