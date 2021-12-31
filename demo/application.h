#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Application
{
public:
    Application(const unsigned int width, const unsigned int height, const char* title);
    void Run();

private:
    void Init();
    void InitRender();
    void Update();
    void Shutdown();

    void ProcessInput(GLFWwindow* window);
    static void ResizeCallback(GLFWwindow* window, int width, int height);
    static void ErrorCallback(int error_code, const char* description);

private:
    GLFWwindow* window_;
    unsigned int width_;
    unsigned int height_;
    //TODO: move to render
    unsigned int VAO_;
    unsigned int VBO_;
    unsigned int EBO_;
    unsigned int texture_;

    const char* kTitleWindow;
};

#endif