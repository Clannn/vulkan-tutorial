#include "application.h"

#include <GLFW/glfw3.h>
#include <volk.h>

#include "logging.h"

void Application::run()
{
    init_window();
    init_vulkan();
    main_loop();
    clearup();
}

void Application::init_window()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "hello vulkan", nullptr, nullptr);
}

void Application::init_vulkan()
{
    if(volkInitialize() != VK_SUCCESS)
    {
        LOGE("volk initialized faild!");
    }
}

void Application::main_loop()
{
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
    }
}

void Application::clearup()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}