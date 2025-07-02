#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    enum class Direction { Forward, Backward, Left, Right, Up, Down };
    enum class Mode { Orbit, Fly };

    Camera();

    void move(Direction direction, float deltaTime);
    void rotate(float xoffset, float yoffset, float deltaTime);
    void zoom(float offset);
    void reset();

    void setOrbitTarget(const glm::vec3& target);
    void setMode(Mode mode);

    const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
    glm::vec3 getPosition() const { return m_position; }
    Mode getMode() const { return m_mode; }

    void update(float deltaTime);

private:
    void updateVectors();
    void updateProjection();

    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 m_targetPosition = m_position;
    glm::vec3 m_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float m_yaw = -90.0f;
    float m_pitch = 0.0f;

    float m_movementSpeed = 5.0f;
    float m_mouseSensitivity = 0.1f;
    float m_zoom = 45.0f;
    float m_aspectRatio = 16.0f / 9.0f;

    Mode m_mode = Mode::Orbit;
    glm::vec3 m_orbitTarget = glm::vec3(0.0f);
    float m_orbitDistance = 5.0f;
    float m_orbitYaw = 0.0f;
    float m_orbitPitch = 20.0f;

    glm::mat4 m_viewMatrix = glm::mat4(1.0f);
    glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

    float m_rotationSmoothing = 0.9f;
    float m_positionSmoothing = 0.95f;
};
