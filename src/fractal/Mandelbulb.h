#pragma once
#include "Fractal.h"
#include "../graphics/Shader.h"

class Mandelbulb
{
public:
    Mandelbulb() = default;
    explicit Mandelbulb(FractalParams params) : params(params) {}

    void updateUniforms(Shader& program)
    {
        program.setInt("fractalType", 0);
        program.setInt("iterations", params.iterations);
        program.setFloat("power", params.power);
        program.setVec3("fractalColor", params.color);
        program.setFloat("glowStrength", params.glowStrength);
        program.setFloat("specular", params.specular);
        program.setFloat("aoStrength", params.aoStrength);
        program.setFloat("shadowStrength", params.shadowStrength);
        program.setInt("maxSteps", params.maxSteps);
        program.setFloat("stepScale", params.stepScale);
        program.setFloat("minDistance", params.minDistance);
        program.setBool("enableAO", params.enableAO);
        program.setBool("enableShadows", params.enableShadows);
        program.setBool("enableGlow", params.enableGlow);
    }

    std::string_view getName() const { return "Mandelbulb"; }
    const FractalParams& getParams() const { return params; }
    void setParams(FractalParams p) { params = std::move(p); }
    
    FractalParams params;
};
