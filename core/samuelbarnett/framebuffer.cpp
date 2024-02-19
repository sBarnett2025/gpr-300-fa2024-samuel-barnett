#include "framebuffer.h"

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

		return buffer;
	}

	Framebuffer createShadowBuffer(unsigned int width, unsigned int height)
	{
		Framebuffer buffer;
		buffer.width = width;
		buffer.height = height;

		glCreateFramebuffers(1, &buffer.shadowFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.shadowFbo);
		glGenTextures(1, &buffer.shadowMap);
		glBindTexture(GL_TEXTURE_2D, buffer.shadowMap);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, buffer.width, buffer.height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		return buffer;
	}
}