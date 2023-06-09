#pragma once

#include <cstdint>

#include <volk.h>

struct GLFWwindow;

class Application
{

public:
    void run();

private:
    static const uint32_t WINDOW_WIDTH  = 800;
    static const uint32_t WINDOW_HEIGHT = 600;

private:
    void init_window();
    
    void init_vulkan();

    void main_loop();

    void clearup();

    void create_instance();

    void create_debug_messenger();

private:
    GLFWwindow *window;

    VkInstance instance;

    VkDebugUtilsMessengerEXT debug_messenger;

};