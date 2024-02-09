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

		//for (unsigned int i = 0; i < 8; i++)
		//{
			//glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[i]);
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, buffer.colorBuffer[i], 0);
		//}
		//GLenum attachments[8] = { GL_COLOR_ATTACHMENT0,
			//GL_COLOR_ATTACHMENT1,
			//GL_COLOR_ATTACHMENT2,
			//GL_COLOR_ATTACHMENT3,
			//GL_COLOR_ATTACHMENT4,
			//GL_COLOR_ATTACHMENT5,
			//GL_COLOR_ATTACHMENT6,
			//GL_COLOR_ATTACHMENT7 };
		//glDrawBuffers(8, attachments);
		

		// depth

		glGenTextures(1, &buffer.depthBuffer);
		glBindTexture(GL_TEXTURE_2D, buffer.depthBuffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, buffer.width, buffer.height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.depthBuffer, 0);

		//glGenRenderbuffers(1, &buffer.depthBuffer);
		//glBindRenderbuffer(GL_RENDERBUFFER, buffer.depthBuffer);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, buffer.width, buffer.height);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer.depthBuffer);

		return buffer;
	}

	Framebuffer createShadowBuffer(unsigned int width, unsigned int height)
	{
		Framebuffer buffer;
		buffer.width = width;
		buffer.height = height;

		glCreateFramebuffers(1, &buffer.shadowFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.shadowFbo);
		glGenTextures(1, &buffer.shadowBuffer);
		glBindTexture(GL_TEXTURE_2D, buffer.shadowBuffer);

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