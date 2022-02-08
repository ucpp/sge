#include "shadow_map_fbo.h"

#include "log.h"
#include <glad/glad.h>

namespace sge
{
	ShadowMapFBO::ShadowMapFBO() : FBO(0), id(0), height(0), width(0) { }

	ShadowMapFBO::~ShadowMapFBO()
	{
		destroy();
	}

	bool ShadowMapFBO::initialize(uint32_t width, uint32_t height)
	{
		this->width = width;
		this->height = height;

		glGenFramebuffers(1, &FBO);
		glGenTextures(1, &id);

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

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

		uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::error("Init shadow map frame buffer error : 0x%x\n", status);
			return false;
		}

		return true;
	}

	void ShadowMapFBO::bindForReading(uint32_t texture_unit)
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void ShadowMapFBO::bindForWriting()
	{
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	}

	void ShadowMapFBO::destroy()
	{
		if (FBO != 0)
		{
			glDeleteFramebuffers(1, &FBO);
		}

		if (id != 0)
		{
			glDeleteTextures(1, &id);
		}
	}
}