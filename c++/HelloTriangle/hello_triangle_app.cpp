#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

#include <hello_triangle_app.hpp>


void hello_triangle_application::run() {
    _init_window();
    _init_vulkan();
    _main_loop();
    _cleanup();
}

void hello_triangle_application::_init_window() {
    glfwInit();
    // tell GLFW not to create an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // disable window resizing for now
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // create the window
    _window = glfwCreateWindow(_width, _hight, "Vulkan", nullptr, nullptr);
}

bool hello_triangle_application::_check_validation_layer_support() {
    // gather info about available validation layers
    uint32_t __layer_count;
    vkEnumerateInstanceLayerProperties(&__layer_count, nullptr);
    std::vector<VkLayerProperties> __available_layers(__layer_count);
    vkEnumerateInstanceLayerProperties(&__layer_count, __available_layers.data());
    // compare the acquired list of available validation
    // layer to the list of requested ones
    for (const char* __layer_name : _validation_layers) {
        bool __layer_found = false;
        for (const auto& __layer_properties : __available_layers) {
            if (strcmp(__layer_name, __layer_properties.layerName) == 0) {
                __layer_found = true;
                break;
            }
        }
        if (!__layer_found) {
            return false;
        }
    }
    return true;
}

void hello_triangle_application::_setup_debug_messenger() {
    if (!_enable_validation_layers) return;

    VkDebugUtilsMessengerCreateInfoEXT __create_info{};
    __create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    __create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    __create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    __create_info.pfnUserCallback = _debug_callback;
    __create_info.pUserData = nullptr; // optional
}

std::vector<const char*> hello_triangle_application::_get_required_extensions() {
    uint32_t __glfw_extension_count = 0;
    const char** __glfw_extensions;
    __glfw_extensions = glfwGetRequiredInstanceExtensions(&__glfw_extension_count);

    std::vector<const char*> __extensions(__glfw_extensions, __glfw_extensions + __glfw_extension_count);
    if (_enable_validation_layers) {
        __extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return __extensions;
}

void hello_triangle_application::_create_instance() {
    if (_enable_validation_layers && !_check_validation_layer_support()) {
        throw std::runtime_error("some of the requested validation layers are unavailable!");
    }
    /* Fill out application information for the Vulkan instance */
    VkApplicationInfo __app_info{};
    __app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    __app_info.pApplicationName = "Hello Triangle";
    __app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    __app_info.pEngineName = "No Engine";
    __app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    __app_info.apiVersion = VK_API_VERSION_1_0;
    /* Fill out Vulkan instance information */
    VkInstanceCreateInfo __create_info{};
    __create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    __create_info.pApplicationInfo = &__app_info;
    // request necessary extensions from GLFW
    uint32_t __glfw_extesion_count = 0;
    const char** __glfw_extensions;
    __glfw_extensions = glfwGetRequiredInstanceExtensions(&__glfw_extesion_count);
    // query all availible extentions
    /*
    uint32_t extention_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extention_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extention_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extention_count, extensions.data());
    */
    /*
    // display extension data
    std::cout << "availible extensions:\n";
    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
    std::cout << std::flush;
    */
    // add extension info for the Vulkan instance 
    auto __extensions = _get_required_extensions();
    __create_info.enabledExtensionCount = static_cast<uint32_t>(__extensions.size());
    __create_info.ppEnabledExtensionNames = __extensions.data();
    // enable requested validation layers, if any
    if (_enable_validation_layers) {
        __create_info.enabledLayerCount = static_cast<uint32_t>(_validation_layers.size());
        __create_info.ppEnabledLayerNames = _validation_layers.data();
    } else {
        __create_info.enabledLayerCount = 0;
    }
    /* Create the Vulkan instance */
    // NOTE: custom allocator callbacks will not be used throughout
    //       this tutorial, and for this reason the second argument
    //       will always be nullptr.
    if (vkCreateInstance(&__create_info, nullptr, &_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create a Vulkan instance!");
    }
}

void hello_triangle_application::_init_vulkan() {
    _create_instance();
}

void hello_triangle_application::_main_loop() {
    while (!glfwWindowShouldClose(_window)) {
        glfwPollEvents();
    }
}

void hello_triangle_application::_cleanup() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}