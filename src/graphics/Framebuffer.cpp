#include "Framebuffer.h"
#include <iostream>

Framebuffer::Framebuffer() = default;

Framebuffer::~Framebuffer()
{
    cleanup();
}

bool Framebuffer::create(int width, int height, bool withDepth, int samples)
{
    m_width = width;
    m_height = height;
    m_samples = samples;
    m_hasDepth = withDepth;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    if (samples > 0)
    {
        glGenTextures(1, &m_colorTexture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_colorTexture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_colorTexture, 0);
        glGenTextures(1, &m_colorTexture);
        glBindTexture(GL_TEXTURE_2D, m_colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);
    }

    if (withDepth)
    {
        if (samples > 0)
        {
            glGenTextures(1, &m_depthTexture);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_depthTexture);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_DEPTH24_STENCIL8, width, height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_depthTexture, 0);
        }
        else
        {
            glGenTextures(1, &m_depthTexture);
            glBindTexture(GL_TEXTURE_2D, m_depthTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
        }
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer not complete!" << std::endl;
        cleanup();
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void Framebuffer::resize(int width, int height)
{
    if (width == m_width && height == m_height) return;
    cleanup();
    create(width, height, m_hasDepth, m_samples);
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void Framebuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bindColorTexture(int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    if (m_samples > 0)
    {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_colorTexture);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    }
}

void Framebuffer::cleanup()
{
    if (m_colorTexture)
    {
        glDeleteTextures(1, &m_colorTexture);
        m_colorTexture = 0;
    }
    if (m_depthTexture)
    {
        glDeleteTextures(1, &m_depthTexture);
        m_depthTexture = 0;
    }
    if (m_fbo)
    {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
}

void Framebuffer::unbindAll()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
