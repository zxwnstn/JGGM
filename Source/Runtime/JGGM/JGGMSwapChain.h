#pragma once

#include "JGGM.h"

class JGGMTexture;

class JGGMSwapChain
{
public:


	JGGMTexture* Present();

private:

	JGGMTexture* BackBuffer;
};