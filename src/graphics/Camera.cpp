#include "Camera.h"
#include <cmath>
#include <GLFW/glfw3.h>
#include <algorithm>

Camera::Camera(glm::vec3 position, glm::vec3 target)
    : m_position(position), m_target(target)
{
    glm::vec3 direction = m_target - m_position;
    m_distance = glm::length(direction);
    direction = glm::normalize(direction);
    m_pitch = asin(direction.y);
    m_yaw = atan2(direction.z, direction.x);
    updateVectors();
}

void Camera::orbit(float dx, float dy)
{
    m_yaw += dx * 0.01f;
    m_pitch += dy * 0.01f;
    
    const float limit = glm::radians(89.0f);
    m_pitch = std::clamp(m_pitch, -limit, limit);
    
    updatePosition();
}

void Camera::pan(float dx, float dy)
{
    glm::vec3 forward = glm::normalize(m_target - m_position);
    glm::vec3 right = glm::normalize(glm::cross(forward, m_up));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));
    
    m_target -= right * dx * m_distance * 0.001f;
    m_target += up * dy * m_distance * 0.001f;
    
    updatePosition();
}

void Camera::zoom(float amount)
{
    m_fov -= amount;
    m_fov = std::clamp(m_fov, 1.0f, 90.0f);
}

void Camera::update()
{
    updateVectors();
}

void Camera::handleMouseMove(double x, double y)
{
    if (m_firstMouse)
    {
        m_lastX = x;
        m_lastY = y;
        m_firstMouse = false;
        return;
    }
    
    float dx = static_cast<float>(x - m_lastX);
    float dy = static_cast<float>(y - m_lastY);
    m_lastX = x;
    m_lastY = y;
    
    if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            pan(dx, dy);
        } else {
            orbit(dx, dy);
        }
    }
}

void Camera::handleScroll(double yoffset)
{
    zoom(static_cast<float>(yoffset) * 0.1f);
}

void Camera::updatePosition()
{
    glm::vec3 direction;
    direction.x = cos(m_yaw) * cos(m_pitch);
    direction.y = sin(m_pitch);
    direction.z = sin(m_yaw) * cos(m_pitch);
    
    m_position = m_target - direction * m_distance;
}

void Camera::updateVectors()
{
    glm::vec3 direction;
    direction.x = cos(m_yaw) * cos(m_pitch);
    direction.y = sin(m_pitch);
    direction.z = sin(m_yaw) * cos(m_pitch);
    
    glm::vec3 right = glm::normalize(glm::cross(direction, m_up));
    m_up = glm::normalize(glm::cross(right, direction));
}
