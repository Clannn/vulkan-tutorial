#include <cstdint>

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

private:
    GLFWwindow *window;
};