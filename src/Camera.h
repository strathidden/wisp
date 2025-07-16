#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

class Camera
{
public:
    Camera(glm::vec3 target = glm::vec3(0.0f), float distance = 3.0f, float azimuth = 0.0f, float elevation = 30.0f);
    
    void processMouseMovement(float xoffset, float yoffset, int button);
    void processMouseScroll(float yoffset);
    void updatePosition();

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float width, float height) const;
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getTarget() const { return m_target; }

private:
    glm::vec3 m_position;
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_target;
    
    float m_distance;
    float m_azimuth;
    float m_elevation;


    const float m_panSpeed = 0.01f;
    const float m_rotateSpeed = 0.5f;
    const float m_zoomSpeed = 0.5f;
    const float m_minDistance = 0.1f;
    const float m_maxElevation = 89.0f;
    const float m_minElevation = -89.0f;
};
