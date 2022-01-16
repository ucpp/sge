#include "render_texture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine
{
    void RenderTexture::Init(int size)
    {
        size_ = size;
        glGenFramebuffers(1, &FBO_);
        glGenTextures(1, &id_);
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size_, size_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id_, 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderTexture::Bind()
    {
        glViewport(0, 0, size_, size_);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    }

    void RenderTexture::BindTexture(int texture_number)
    {
        glActiveTexture(GL_TEXTURE0 + texture_number);
        glBindTexture(GL_TEXTURE_2D, id_);
    }

    void RenderTexture::Clear()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}