#include "framebuffer.h"
#include <iostream>

namespace samuelbarnett
{
	Framebuffer createFramebuffer(unsigned int width, unsigned int height, int colorFormat)
	{
		
		Framebuffer buffer;
		buffer.width = width;
		buffer.height = height;

		glGenFramebuffers(1, &buffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.fbo);

		// color
		glGenTextures(1, &buffer.colorBuffer);
		glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, colorFormat, buffer.width, buffer.height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.colorBuffer, 0);
		
		// depth

		glGenTextures(1, &buffer.depthBuffer);
		glBindTexture(GL_TEXTURE_2D, buffer.depthBuffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, buffer.width, buffer.height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.depthBuffer, 0);


		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Framebuffer incomplete: %d " << fboStatus << std::endl;
		}
		return buffer;
	}

	Framebuffer createShadowBuffer(unsigned int width, unsigned int height)
	{
		Framebuffer buffer;
		buffer.width = width;
		buffer.height = height;

		unsigned int shadowFbo;
		unsigned int shadowDepth;

		glGenFramebuffers(1, &shadowFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);

		glGenTextures(1, &shadowDepth);
		glBindTexture(GL_TEXTURE_2D, shadowDepth);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, buffer.width, buffer.height);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepth, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		float borderColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		buffer.fbo = shadowFbo;
		buffer.depthBuffer = shadowDepth;

		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Framebuffer incomplete: %d " << fboStatus << std::endl;
		}

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		return buffer;
	}

	Gbuffer createGBuffer(unsigned int width, unsigned int height)
	{
		Gbuffer buffer;
		buffer.width = width;
		buffer.height = height;


		glCreateFramebuffers(1, &buffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.fbo);


		int formats[3] =
		{
			GL_RGB32F,
			GL_RGB16F,
			GL_RGB16F
		};


		for (size_t i = 0; i < 3; i++)
		{
			glGenTextures(1, &buffer.colorBuffers[i]);
			glBindTexture(GL_TEXTURE_2D, buffer.colorBuffers[i]);
			glTexStorage2D(GL_TEXTURE_2D, 1, formats[i], width, height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, buffer.colorBuffers[i], 0);
		}

		const GLenum drawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 }; 

		glDrawBuffers(3, drawBuffers);


		glGenTextures(1, &buffer.depthBuffer);
		glBindTexture(GL_TEXTURE_2D, buffer.depthBuffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, buffer.width, buffer.height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.depthBuffer, 0);

		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Framebuffer incomplete: %d " << fboStatus << std::endl;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		

		return buffer;
	}

}