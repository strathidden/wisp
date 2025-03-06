#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::string loadShaderSource(const char* filePath) {
    std::ifstream in(filePath, std::ios::in);
    if (!in.is_open()) {
        std::cerr << "Failed to open " << filePath << std::endl;
        exit(EXIT_FAILURE);
    }
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

unsigned int compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile "
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader!" << std::endl;
        std::cerr << message << std::endl;
        delete[] message;
        glDeleteShader(id);
        exit(EXIT_FAILURE);
    }
    return id;
}

unsigned int createShader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexSource = loadShaderSource(vertexPath);
    std::string fragmentSource = loadShaderSource(fragmentPath);

    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 4.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float lastX = 400, lastY = 300;
bool firstMouse = true;
float yaw   = -90.0f;
float pitch = 0.0f;

bool showCursor = false;

void processInput(GLFWwindow* window, float deltaTime) {
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        showCursor = true;
    if(glfwGetKey(window, GLFW_KEY_F) != GLFW_PRESS)
        showCursor = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (showCursor)
        return;

    if(firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }
    
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;
    
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw   += xoffset;
    pitch += yoffset;
    
    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int main() {
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Wisp", NULL, NULL);
    if(!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 1920, 1080);
    
    glfwSetCursorPosCallback(window, mouse_callback);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    unsigned int shaderProgram = createShader("shaders/shader.vert", "shaders/shader.frag");
    
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
    
    int maxSteps = 100;
    float epsilon = 0.001f;
    float maxDistance = 100.0f;
    int iterations = 8;
    float bailout = 2.0f;
    float power = 8.0f;
    
    float lastFrame = 0.0f;
    
    while(!glfwWindowShouldClose(window)) {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            break;

        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window, deltaTime);
        
        glfwSetInputMode(window, GLFW_CURSOR, showCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        {
            ImGui::Begin("Parameters");
            ImGui::SliderInt("Max Steps", &maxSteps, 50, 500);
            ImGui::SliderFloat("Epsilon", &epsilon, 0.0001f, 0.01f, "%.5f");
            ImGui::SliderFloat("Max Distance", &maxDistance, 10.0f, 500.0f);
            ImGui::SliderInt("Iterations", &iterations, 1, 20);
            ImGui::SliderFloat("Bailout", &bailout, 1.0f, 10.0f);
            ImGui::SliderFloat("Power", &power, 2.0f, 16.0f);
            ImGui::End();
        }
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        
        int resolutionLoc = glGetUniformLocation(shaderProgram, "u_resolution");
        glUniform2f(resolutionLoc, 1920.0f, 1080.0f);
        
        int timeLoc = glGetUniformLocation(shaderProgram, "u_time");
        glUniform1f(timeLoc, currentFrame);
        
        int cameraPosLoc = glGetUniformLocation(shaderProgram, "u_cameraPos");
        glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
        
        glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
        glm::vec3 up = glm::normalize(glm::cross(right, cameraFront));
        glm::mat3 cameraRot(right, up, -cameraFront);
        int cameraRotLoc = glGetUniformLocation(shaderProgram, "u_cameraRot");
        glUniformMatrix3fv(cameraRotLoc, 1, GL_FALSE, glm::value_ptr(cameraRot));
        
        glUniform1i(glGetUniformLocation(shaderProgram, "u_maxSteps"), maxSteps);
        glUniform1f(glGetUniformLocation(shaderProgram, "u_epsilon"), epsilon);
        glUniform1f(glGetUniformLocation(shaderProgram, "u_maxDistance"), maxDistance);
        glUniform1i(glGetUniformLocation(shaderProgram, "u_iterations"), iterations);
        glUniform1f(glGetUniformLocation(shaderProgram, "u_bailout"), bailout);
        glUniform1f(glGetUniformLocation(shaderProgram, "u_power"), power);
        
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteProgram(shaderProgram);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
