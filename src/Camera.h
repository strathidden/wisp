#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement { UP, DOWN, LEFT, RIGHT, PAN, ZOOM };

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
    glm::vec3 m_up;
    glm::vec3 m_target;
    
    float m_distance;
    float m_azimuth;
    float m_elevation;
    float m_panSpeed;
    float m_rotateSpeed;
    float m_zoomSpeed;
};
