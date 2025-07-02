#pragma once
#include <glad/glad.h>
#include <vector>

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();

    bool create(int width, int height, bool withDepth, int samples = 0);
    void resize(int width, int height);
    void bind() const;
    void unbind() const;
    void bindColorTexture(int unit = 0) const;
    void bindDepthTexture(int unit = 0) const;

    static void unbindAll();

private:
    void cleanup();

    GLuint m_fbo = 0;
    GLuint m_colorTexture = 0;
    GLuint m_depthTexture = 0;
    int m_width = 0;
    int m_height = 0;
    int m_samples = 0;
    bool m_hasDepth = false;
};
