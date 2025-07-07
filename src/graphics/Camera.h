#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera(glm::vec3 position = {0.0f, 0.0f, 5.0f}, glm::vec3 target = {0.0f, 0.0f, 0.0f});

    void orbit(float dx, float dy);
    void pan(float dx, float dy);
    void zoom(float amount);
    void update();

    glm::mat4 getViewMatrix() const { return glm::lookAt(m_position, m_target, m_up); }
    glm::mat4 getProjectionMatrix(float aspect) const { return glm::perspective(glm::radians(m_fov), aspect, 0.1f, 100.0f); }
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getTarget() const { return m_target; }
    float getFov() const { return m_fov; }

    void setPosition(const glm::vec3& position) { m_position = position; }
    void setTarget(const glm::vec3& target) { m_target = target; }
    void setFov(float fov) { m_fov = fov; }

    void handleMouseMove(double x, double y);
    void handleScroll(double yoffset);

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up = {0.0f, 1.0f, 0.0f};
    float m_distance = 5.0f;
    float m_pitch = 0.0f;
    float m_yaw = 0.0f;
    float m_fov = 45.0f;
    
    double m_lastX = 0.0;
    double m_lastY = 0.0;
    bool m_firstMouse = true;
    
    void updatePosition();
    void updateVectors();
};
