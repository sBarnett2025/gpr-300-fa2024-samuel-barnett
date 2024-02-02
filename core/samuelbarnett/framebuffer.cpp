#include "framebuffer.h"
#include "../ew/external/glad.h"

namespace samuelbarnett
{
	Framebuffer createFramebuffer(unsigned int width, unsigned int height, int colorFormat)
	{
		Framebuffer buffer;

		glGenFramebuffers(1, &buffer.fbo);

		glBindFramebuffer(GL_FRAMEBUFFER, buffer.fbo);


	}


}