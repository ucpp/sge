#include "cascaded_shadow_map_fbo.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "log.h"
#include "engine_common.h"
#include <cassert>

namespace sge
{
	CascadedShadowMapFBO::CascadedShadowMapFBO() : width(0), height(0), FBO(0)
	{
		for (auto& item : ids)
		{
			item = 0;
		}
	}

	CascadedShadowMapFBO::~CascadedShadowMapFBO()
	{
		destroy();
	}

	void CascadedShadowMapFBO::destroy()
	{
		if (FBO != 0)
		{
			glDeleteFramebuffers(1, &FBO);
		}

		glDeleteTextures(count_maps, ids);
	}

	bool CascadedShadowMapFBO::initialize(uint32_t width, uint32_t height)
	{
		this->width = width;
		this->height = height;

		glGenFramebuffers(1, &FBO);
		glGenTextures(count_maps, ids);

		for (uint32_t i = 0; i < count_maps; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, ids[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ids[0], 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::error("Init cascaded shadow map frame buffer error : 0x%x\n", status);
			return false;
		}

		return true;
	}

	void CascadedShadowMapFBO::bindForWriting(uint32_t index)
	{
		assert(index < count_maps);
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ids[index], 0);
	}

	void CascadedShadowMapFBO::bindForReading()
	{
		glActiveTexture(SHADOW_TEXTURE_UNIT0);
		glBindTexture(GL_TEXTURE_2D, ids[0]);

		glActiveTexture(SHADOW_TEXTURE_UNIT1);
		glBindTexture(GL_TEXTURE_2D, ids[1]);

		glActiveTexture(SHADOW_TEXTURE_UNIT2);
		glBindTexture(GL_TEXTURE_2D, ids[2]);
	}
}