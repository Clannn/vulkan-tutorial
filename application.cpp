#include "application.h"

#include <vector>
#include <unordered_set>
#include <cstring>

#include <GLFW/glfw3.h>
#include <volk.h>

#include "logging.h"

#ifdef NDEBUG
    const bool enable_validation_layers = false;
#else
    const bool enable_validation_layers = true;
#endif

const std::vector<const char*> requested_validation_layers = {
    "VK_LAYER_KHRONOS_validation",
};

// helper functions
namespace
{
bool check_validation_layer_support(const std::vector<const char*>& requested_validation_layers)
{
    uint32_t enabled_layer_count = 0;
    vkEnumerateInstanceLayerProperties(&enabled_layer_count, nullptr);
    std::vector<VkLayerProperties> enabled_layer_properties(enabled_layer_count);
    vkEnumerateInstanceLayerProperties(&enabled_layer_count, enabled_layer_properties.data());

    bool check = true;
    for (const auto& layer : requested_validation_layers)
    {
        auto it =  std::find_if(enabled_layer_properties.begin(), enabled_layer_properties.end(), [layer](const VkLayerProperties& layer_property){
            return strcmp(layer, layer_property.layerName) == 0;
        });
        if(it == enabled_layer_properties.end())
        {
            check = false;
            break;
        }
    }

    if(!check)
    {
        LOGE("There are some requested validation layers can not be enabled!");
    }
    return check;
}

std::vector<const char *> get_required_extensions()
{
    uint32_t glfw_extension_count = 0;
    const char* const * glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

    // Beginning with the 1.3.216 Vulkan SDK, the VK_KHR_PORTABILITY_subset extension is mandatory.
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    if(enable_validation_layers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

/// @brief the debug callback function
VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT              messageType,
    const VkDebugUtilsMessengerCallbackDataEXT*  pCallbackData,
    void*                                        pUserData
)
{
    LOGE("validation layer: {}", pCallbackData->pMessage);

    return VK_FALSE;
}

///@brief vkCreateDebugUtilsMessengerEXT function should be finded manully
VkResult create_debug_utils_messengerEXT(VkInstance                                instance, 
                                         const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                         const VkAllocationCallbacks*              pAllocator,
                                         VkDebugUtilsMessengerEXT*                 pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

///@brief vkDestroyDebugUtilsMessengerEXT function should be finded manully
void destroy_debug_utils_messengerEXT(VkInstance                                instance, 
                                      VkDebugUtilsMessengerEXT                  debugMessenger,
                                      const VkAllocationCallbacks*              pAllocator)
{
    if(!enable_validation_layers) return;

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr)
    {
        return func(instance, debugMessenger, pAllocator);
    }
    else
    {
        LOGE("destroy debug utils messenger failed!");
    }
}

VkDebugUtilsMessengerCreateInfoEXT get_debug_messenger_create_info()
{
    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    debug_messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_info.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_info.pfnUserCallback = debug_callback;
    debug_messenger_info.pUserData       = nullptr;
    return debug_messenger_info;
}
}  //namespace 

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

    create_debug_messenger();
}

void Application::main_loop()
{
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
    }
}

void Application::clearup()
{
    destroy_debug_utils_messengerEXT(instance, debug_messenger, nullptr);

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

    //validation layers
    if(enable_validation_layers && check_validation_layer_support(requested_validation_layers))
    {
        instance_info.enabledLayerCount = static_cast<uint32_t>(requested_validation_layers.size());
        instance_info.ppEnabledLayerNames = requested_validation_layers.data();

        // pass a pointer to a VkDebugUtilsMessengerCreateInfoEXT struct in the pNext field
        // to debug vkCreateInstance and vkDestroyInstance calls.
        VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info = get_debug_messenger_create_info();
        instance_info.pNext = &debug_messenger_info;
    }
    else
    {
        instance_info.enabledLayerCount = 0;
        instance_info.ppEnabledLayerNames = nullptr;
    }
    
    //prepare required instance extensions
    std::vector<const char*> extensions = get_required_extensions();

    instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
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

void Application::create_debug_messenger()
{
    if(!enable_validation_layers) return;

    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info = get_debug_messenger_create_info();

    if( create_debug_utils_messengerEXT(instance, &debug_messenger_info, nullptr, &debug_messenger) != VK_SUCCESS)
    {
        LOGE("create debug utils messenger failed!");
    }
}