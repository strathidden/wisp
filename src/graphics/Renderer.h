#pragma once
#include "Shader.h"
#include "Camera.h"
#include "../fractal/Fractal.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

class Renderer
{
public:
    Renderer(GLFWwindow* window, int width, int height);
    ~Renderer();

    void resize(int width, int height);
    void render(const Camera& camera);

    template<typename Fractal>
    void setFractal(Fractal fractal)
    {
        m_currentFractal = std::make_unique<FractalWrapper<Fractal>>(std::move(fractal));
    }
    
    bool isMandelbulbActive() const;
    bool isJuliaActive() const;
    
    std::string_view getCurrentFractalName() const;
    const FractalParams& getCurrentFractalParams() const;
    void setCurrentFractalParams(const FractalParams& params);

private:
    struct FractalBase
    {
        virtual ~FractalBase() = default;
        virtual void updateUniforms(Shader& program) = 0;
        virtual std::string_view getName() const = 0;
        virtual const FractalParams& getParams() const = 0;
        virtual void setParams(FractalParams params) = 0;
    };

    template<typename Fractal>
    struct FractalWrapper : FractalBase
    {
        Fractal fractal;
        FractalWrapper(Fractal f) : fractal(std::move(f)) {}
        void updateUniforms(Shader& program) override { fractal.updateUniforms(program); }
        std::string_view getName() const override { return fractal.getName(); }
        const FractalParams& getParams() const override { return fractal.getParams(); }
        void setParams(FractalParams params) override { fractal.setParams(std::move(params)); }
    };

    GLFWwindow* m_window;
    int m_width, m_height;
    
    GLuint m_screenVAO, m_screenVBO;
    GLuint m_outputTexture;
    Shader m_computeShader;
    Shader m_screenShader;
    
    std::unique_ptr<FractalBase> m_currentFractal;

    void createScreenQuad();
    void createOutputTexture();
    void updateFrameStats();
};
