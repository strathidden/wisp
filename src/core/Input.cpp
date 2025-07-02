#include "Input.h"

Input::Input(GLFWwindow* window) : m_window(window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

void Input::update()
{
    m_lastCursorPos = m_cursorPos;
    m_scrollDelta = 0;
}

bool Input::isKeyPressed(int key) const
{
    auto it = m_keyStates.find(key);
    return it != m_keyStates.end() && it->second;
}

bool Input::isMouseButtonPressed(int button) const
{
    auto it = m_mouseStates.find(button);
    return it != m_mouseStates.end() && it->second;
}

glm::dvec2 Input::getCursorPosition() const
{
    return m_cursorPos;
}

glm::dvec2 Input::getCursorDelta() const
{
    return m_cursorPos - m_lastCursorPos;
}

double Input::getScrollDelta() const
{
    return m_scrollDelta;
}

void Input::registerKeyCallback(KeyCallback callback)
{
    m_keyCallbacks.push_back(std::move(callback));
}

void Input::registerMouseButtonCallback(MouseButtonCallback callback)
{
    m_mouseButtonCallbacks.push_back(std::move(callback));
}

void Input::registerScrollCallback(ScrollCallback callback)
{
    m_scrollCallbacks.push_back(std::move(callback));
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    input->m_keyStates[key] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    
    for (const auto& cb : input->m_keyCallbacks)
    {
        cb(key, action);
    }
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    input->m_mouseStates[button] = (action == GLFW_PRESS);
    
    for (const auto& cb : input->m_mouseButtonCallbacks)
    {
        cb(button, action);
    }
}

void Input::cursorCallback(GLFWwindow* window, double xpos, double ypos)
{
    auto* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    input->m_cursorPos = glm::dvec2(xpos, ypos);
}

void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    input->m_scrollDelta += yoffset;
    
    for (const auto& cb : input->m_scrollCallbacks)
    {
        cb(xoffset, yoffset);
    }
}
