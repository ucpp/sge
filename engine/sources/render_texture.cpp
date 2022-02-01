#include "render_texture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace sge
{
	void RenderTexture::initialize(int sz)
	{
		size = sz;
		glGenFramebuffers(1, &FBO);
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderTexture::bind()
	{
		glViewport(0, 0, size, size);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	}

	void RenderTexture::bindTexture(int texture_number)
	{
		glActiveTexture(GL_TEXTURE0 + texture_number);
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void RenderTexture::clear()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}
}