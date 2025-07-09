#include "Camera.h"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 target)
    : m_position(position),
    m_worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
    m_yaw(-90.0f),
    m_pitch(0.0f),
    m_movementSpeed(2.5f),
    m_mouseSensitivity(0.1f),
    m_zoom(45.0f),
    m_firstMouse(true)
{
    m_target = target;
    m_front = glm::normalize(m_target - m_position);
    updateVectors();
}

void Camera::processKeyboard(CameraMovement direction)
{
    float velocity = m_movementSpeed * 0.01f;

    if (direction == FORWARD) m_position += m_front * velocity;
    if (direction == BACKWARD) m_position -= m_front * velocity;
    if (direction == LEFT) m_position -= m_right * velocity;
    if (direction == RIGHT) m_position += m_right * velocity;
    
    m_front = glm::normalize(m_target - m_position);
    updateVectors();
}

void Camera::processMouseMovement(float xpos, float ypos)
{
    if (m_firstMouse)
    {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos;
    m_lastX = xpos;
    m_lastY = ypos;

    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    updateVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    m_zoom -= yoffset;
    if (m_zoom < 1.0f) m_zoom = 1.0f;
    if (m_zoom > 90.0f) m_zoom = 90.0f;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix(float width, float height) const
{
    float aspect = (height > 0) ? width / height : 1.0f;
    return glm::perspective(glm::radians(m_zoom), aspect, 0.1f, 100.0f);
}

void Camera::updateVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
