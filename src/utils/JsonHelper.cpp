#include "JsonHelper.h"
#include <fstream>
#include <iostream>

void JsonHelper::saveConfig(const std::string& path, const IFractal& fractal, const Camera& camera)
{
    nlohmann::json j;
    j["fractal"] = fractal.toJson();
    j["camera"] = {
        {"position", {camera.getPosition().x, camera.getPosition().y, camera.getPosition().z}},
        {"target", {camera.getTarget().x, camera.getTarget().y, camera.getTarget().z}},
        {"fov", camera.getFov()}
    };
    
    std::ofstream file(path);
    if (file)
    {
        file << j.dump(4);
    }
}

void JsonHelper::loadConfig(const std::string& path, IFractal& fractal, Camera& camera)
{
    std::ifstream file(path);
    if (!file)
    {
        return;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        if (j.contains("fractal"))
        {
            fractal.fromJson(j["fractal"]);
        }
        
        if (j.contains("camera"))
        {
            auto cam = j["camera"];
            if (cam.contains("position") && cam["position"].is_array() && cam["position"].size() == 3)
            {
                glm::vec3 pos(cam["position"][0], cam["position"][1], cam["position"][2]);
                camera.setPosition(pos);
            }
            if (cam.contains("target") && cam["target"].is_array() && cam["target"].size() == 3)
            {
                glm::vec3 target(cam["target"][0], cam["target"][1], cam["target"][2]);
                camera.setTarget(target);
            }
            if (cam.contains("fov"))
            {
                camera.setFov(cam["fov"]);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
    }
}
