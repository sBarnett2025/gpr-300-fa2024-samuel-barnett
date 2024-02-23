#pragma once
#include "../ew/external/glad.h"
namespace samuelbarnett
{
	struct Framebuffer
	{
		unsigned int fbo;
		unsigned int shadowFbo;
		unsigned int colorBuffer;
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
	};
	Framebuffer createFramebuffer(unsigned int width, unsigned int height, int colorFormat); 

	Framebuffer createShadowBuffer(unsigned int width, unsigned int height);

	struct Gbuffer
	{
		unsigned int fbo;
		unsigned int shadowFbo;
		unsigned int colorBuffers[8];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
	};

	Gbuffer createGBuffer(unsigned int width, unsigned int height);
}
