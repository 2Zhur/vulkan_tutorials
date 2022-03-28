#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

class hello_triangle_application
{
public:
    void run();

private:
    void _init_window();

    static VKAPI_ATTR VkBool32 VKAPI_CALL _debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data) {
        
        std::cerr << "validatoin layer: " << callback_data->pMessage << std::endl;
        return VK_FALSE;
    }

    bool _check_validation_layer_support();
    void _setup_debug_messenger();
    void _create_instance();
    void _init_vulkan();
    void _main_loop();
    void _cleanup();

    std::vector<const char*> _get_required_extensions();

    /* Private members */
    // Vulkan instance
    VkInstance _instance;
    // the application window and its parameters
    GLFWwindow* _window;
    const uint32_t _width = 800;
    const uint32_t _hight = 600;
    // validation layers
    const std::vector<const char*> _validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };
    // debug messenger function handle
    VkDebugUtilsMessengerEXT _debug_messenger;
    // validation layers on/off switch
#ifdef NDEBUG
    const bool _enable_validation_layers = false;
#else
    const bool _enable_validation_layers = true;
#endif
};