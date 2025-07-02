#include "Camera.h"
#include <algorithm>

Camera::Camera()
{
    updateVectors();
    updateProjection();
}

void Camera::update(float deltaTime)
{
    m_position = glm::mix(m_position, m_targetPosition, 1.0f - std::pow(1.0f - m_positionSmoothing, deltaTime * 60.0f));

    updateVectors();
    updateProjection();
}

void Camera::move(Direction direction, float deltaTime)
{   
    float velocity = m_movementSpeed * deltaTime;

    if (m_mode == Mode::Fly)
    {
        switch (direction)
        {
            case Direction::Forward: m_targetPosition += m_front * velocity; break;
            case Direction::Backward: m_targetPosition -= m_front * velocity; break;
            case Direction::Left: m_targetPosition -= m_right * velocity; break;
            case Direction::Right: m_targetPosition += m_right * velocity; break;
            case Direction::Up: m_targetPosition += m_up * velocity; break;
            case Direction::Down: m_targetPosition -= m_up * velocity; break;
        }
    }
    else
    {
        switch (direction)
        {
            case Direction::Forward: m_orbitTarget += m_front * velocity; break;
            case Direction::Backward: m_orbitTarget -= m_front * velocity; break;
            case Direction::Left: m_orbitTarget -= m_right * velocity; break;
            case Direction::Right: m_orbitTarget += m_right * velocity; break;
            case Direction::Up: m_orbitTarget += m_up * velocity; break;
            case Direction::Down: m_orbitTarget -= m_up * velocity; break;
        }
    }
}

void Camera::rotate(float xoffset, float yoffset, float deltaTime)
{
    if (m_mode == Mode::Orbit)
    {
        m_orbitYaw += xoffset * m_mouseSensitivity;
        m_orbitPitch += yoffset * m_mouseSensitivity;
        m_orbitPitch = std::clamp(m_orbitPitch, -89.0f, 89.0f);
    }
    else
    {
        m_yaw += xoffset * m_mouseSensitivity;
        m_pitch += yoffset * m_mouseSensitivity;
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    }

    updateVectors();
}

void Camera::zoom(float offset)
{
    if (m_mode == Mode::Orbit)
    {
        m_orbitDistance = std::max(m_orbitDistance - offset, 0.1f);
    }
    else
    {
        m_zoom = std::clamp(m_zoom - offset, 1.0f, 90.0f);
    }
}

void Camera::reset()
{
    m_position = glm::vec3(0.0f, 0.0f, 3.0f);
    m_targetPosition = m_position;
    m_front = glm::vec3(0.0f, 0.0f, -1.0f);
    m_yaw = -90.0f;
    m_pitch = 0.0f;
    m_zoom = 45.0f;
    m_orbitTarget = glm::vec3(0.0f);
    m_orbitDistance = 5.0f;
    m_orbitYaw = 0.0f;
    m_orbitPitch = 20.0f;

    updateVectors();
    updateProjection();
}

void Camera::setOrbitTarget(const glm::vec3& target)
{
    m_orbitTarget = target;
}

void Camera::setMode(Mode mode)
{
    m_mode = mode;
}

void Camera::updateVectors()
{
    if (m_mode == Mode::Orbit)
    {
        m_position.x = m_orbitTarget.x + m_orbitDistance * cos(glm::radians(m_orbitYaw)) * cos(glm::radians(m_orbitPitch));
        m_position.y = m_orbitTarget.y + m_orbitDistance * sin(glm::radians(m_orbitPitch));
        m_position.z = m_orbitTarget.z + m_orbitDistance * sin(glm::radians(m_orbitYaw)) * cos(glm::radians(m_orbitPitch));

        m_viewMatrix = glm::lookAt(m_position, m_orbitTarget, m_up);
    }
    else
    { 
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(front);
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        m_up = glm::normalize(glm::cross(m_right, m_front));

        m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
    }
}

void Camera::updateProjection()
{
    m_projectionMatrix = glm::perspective(glm::radians(m_zoom), m_aspectRatio, 0.1f, 100.0f);
}
