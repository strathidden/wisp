#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <optional>
#include <cmath>

class Camera
{
public:
    enum class Action { None, Rotate, Pan, Zoom };

    Camera(glm::vec3 target = {0,0,0}, float distance = 5.0f)
        : m_target(target), m_distance(distance)
    {
        update();
    }

    void start(Action action, glm::vec2 mousePos)
    {
        m_currentAction = action;
        m_lastMousePos = mousePos;
    }

    void end()
    {
        m_currentAction = Action::None;
    }

    void process(glm::vec2 mousePos)
    {
        if (m_currentAction == Action::None) return;

        const glm::vec2 delta = mousePos - m_lastMousePos;
        m_lastMousePos = mousePos;

        switch (m_currentAction)
        {
            case Action::Rotate: rotate(delta * 0.01f); break;
            case Action::Pan: pan(delta * 0.005f); break;
            case Action::Zoom: zoom(delta.y * 0.05f); break;
            default: break;
        }
    }

    void rotate(glm::vec2 delta)
    {
        m_azimuth -= delta.x;
        m_elevation = glm::clamp(m_elevation + delta.y, -glm::pi<float>()/2 + 0.1f, glm::pi<float>()/2 - 0.1f);
        update();
    }

    void pan(glm::vec2 delta)
    {
        const glm::vec3 right = glm::normalize(glm::cross(m_front, m_up));
        const glm::vec3 actualUp = glm::normalize(glm::cross(right, m_front));
        
        m_target -= right * delta.x * m_distance;
        m_target += actualUp * delta.y * m_distance;
        update();
    }

    void zoom(float delta)
    {
        m_distance = glm::clamp(m_distance * (1.0f - delta), 0.5f, 50.0f);
        update();
    }

    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(m_position, m_target, m_up);
    }
    
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getTarget() const { return m_target; }
    float getDistance() const { return m_distance; }
    bool isActive() const { return m_currentAction != Action::None; }

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_front{0,0,-1};
    glm::vec3 m_up{0,1,0};
    float m_distance;

    // horizontal angle
    float m_azimuth = 0.0f;

    // vertical angle
    float m_elevation = 0.0f;

    Action m_currentAction = Action::None;
    glm::vec2 m_lastMousePos{0,0};

    void update()
    {
        m_position = m_target;
        m_position.x += m_distance * cos(m_elevation) * sin(m_azimuth);
        m_position.y += m_distance * sin(m_elevation);
        m_position.z += m_distance * cos(m_elevation) * cos(m_azimuth);
        
        m_front = glm::normalize(m_target - m_position);
        const glm::vec3 right = glm::normalize(glm::cross(m_front, {0,1,0}));
        m_up = glm::normalize(glm::cross(right, m_front));
    }
};
