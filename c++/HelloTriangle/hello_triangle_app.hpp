#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>


VkResult create_debug_utils_messenger(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* create_info,
    const VkAllocationCallbacks* allocator,
    VkDebugUtilsMessengerEXT* debug_messenger);

void destroy_debug_utils_messenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks* allocator);

class hello_triangle_application
{
public:
    void run();

private:
    void init_window();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data) {
        
        std::cerr << "validatoin layer: " << callback_data->pMessage << std::endl;
        return VK_FALSE;
    }

    bool check_validation_layer_support();
    void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);
    void setup_debug_messenger();
    void pick_physical_device();
    void create_instance();
    void init_vulkan();
    void main_loop();
    void cleanup();

    std::vector<const char*> get_required_extensions();

    /* Private members */
    // Vulkan instance
    VkInstance instance;
    // the application window and its parameters
    GLFWwindow* window;
    const uint32_t width = 800;
    const uint32_t hight = 600;
    // validation layers
    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };
    // debug messenger function handle
    VkDebugUtilsMessengerEXT debug_messenger;
    // physical device handle
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    // validation layers on/off switch
#ifdef NDEBUG
    const bool enable_validation_layers = false;
#else
    const bool enable_validation_layers = true;
#endif
};