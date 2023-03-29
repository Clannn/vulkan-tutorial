#include "application.h"

#include <vector>

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
    // This will attempt to load Vulkan loader from the system.
    // If this function fails, this means Vulkan loader isn't installed on your system.
    if(volkInitialize() != VK_SUCCESS)
    {
        LOGE("volk initialized faild!");
    }
    create_instance();
}

void Application::main_loop()
{
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
    }
}

void Application::clearup()
{
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::create_instance()
{
    VkApplicationInfo app_info{VK_STRUCTURE_TYPE_APPLICATION_INFO};

    app_info.pApplicationName   = "Hello Triangle";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName        = nullptr;
    app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion         = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instance_info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};

    // Beginning with the 1.3.216 Vulkan SDK, the VK_KHR_PORTABILITY_subset extension is mandatory.
    instance_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    instance_info.pApplicationInfo = &app_info;
    //for now, just no validation layers
    instance_info.enabledLayerCount = 0;
    instance_info.ppEnabledLayerNames = nullptr;
    
    //prepared required instance extensions
    uint32_t glfw_extension_count = 0;
    const char* const * glfw_extensions;

    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    // Beginning with the 1.3.216 Vulkan SDK, the VK_KHR_PORTABILITY_subset extension is mandatory.
    uint32_t extension_count = glfw_extension_count + 1;
    std::vector<const char*> extensions(glfw_extensions, glfw_extensions+glfw_extension_count);
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    instance_info.enabledExtensionCount = extension_count;
    instance_info.ppEnabledExtensionNames = extensions.data();

    //create instance
    if (vkCreateInstance(&instance_info, nullptr, &instance) != VK_SUCCESS){
        LOGE("vulkan instance created failed!");
    }

    // This function will load all required Vulkan entrypoints, including all extensions;
    // you can use Vulkan from here on as usual.
    volkLoadInstance(instance);

    // log the supported instance extensions
    uint32_t supported_extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &supported_extension_count, nullptr);
    std::vector<VkExtensionProperties> extension_properties(supported_extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &supported_extension_count, extension_properties.data());

    LOGI("There are {} supported instance extensions:", supported_extension_count);
    for (const auto &extension_property : extension_properties)
    {
        LOGI("\t{}", extension_property.extensionName);
    }
}