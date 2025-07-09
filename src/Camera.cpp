#include "Camera.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>

Camera::Camera(glm::vec3 target, float distance, float azimuth, float elevation)
    : m_target(target),
    m_up(glm::vec3(0.0f, 1.0f, 0.0f)),
    m_distance(distance),
    m_azimuth(azimuth),
    m_elevation(elevation),
    m_panSpeed(0.01f),
    m_rotateSpeed(0.5f),
    m_zoomSpeed(0.5f)
{
    updatePosition();
}

void Camera::processMouseMovement(float xoffset, float yoffset, int button)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        m_azimuth -= xoffset * m_rotateSpeed;
        m_elevation += yoffset * m_rotateSpeed;
        
        m_elevation = std::clamp(m_elevation, -89.0f, 89.0f);
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        glm::vec3 viewDir = glm::normalize(m_target - m_position);
        glm::vec3 right = glm::normalize(glm::cross(viewDir, m_up));
        glm::vec3 up = glm::normalize(glm::cross(right, viewDir));
        
        m_target -= right * xoffset * m_panSpeed * m_distance;
        m_target += up * yoffset * m_panSpeed * m_distance;
    }

    updatePosition();
}

void Camera::processMouseScroll(float yoffset)
{
    m_distance *= (1.0f - yoffset * m_zoomSpeed);
    m_distance = std::max(m_distance, 0.1f);
    updatePosition();
}

void Camera::updatePosition()
{
    float azimuthRad = glm::radians(m_azimuth);
    float elevationRad = glm::radians(m_elevation);
    
    m_position.x = m_target.x + m_distance * cos(elevationRad) * sin(azimuthRad);
    m_position.y = m_target.y + m_distance * sin(elevationRad);
    m_position.z = m_target.z + m_distance * cos(elevationRad) * cos(azimuthRad);
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 Camera::getProjectionMatrix(float width, float height) const
{
    float aspect = (height > 0) ? width / height : 1.0f;
    return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}
