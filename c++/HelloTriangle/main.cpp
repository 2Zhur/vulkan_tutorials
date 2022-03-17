#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

/**
 * A quick side note on code styling: I prefer to use
 * snake-case instead of camelback-case. It is consistent
 * with the style of the standard library and I consider it
 * to be more aesthetically pleasing. As a bonus, it allows
 * to better differenciate between API calls and my own code
 */

class hello_triangle_application
{
public:
    void run() {
        init_window();
        init_vulkan();
        main_loop();
        cleanup();
    }

private:
    void init_window() {
        glfwInit();
        // tell GLFW not to create an OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // disable window resizing for now
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        _window = glfwCreateWindow(_width, _hight, "Vulkan", nullptr, nullptr);

    }

    void create_instance() {
        /* Fill out application information for the Vulkan instance */
        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "Hello Triangle";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "No Engine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;

        /* Fill out Vulkan instance information */
        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;

        // request necessary extensions from GLFW
        uint32_t glfw_extesion_count = 0;
        const char** glfw_extensions;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extesion_count);

        // query all availible extentions
        uint32_t extention_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extention_count, nullptr);
        std::vector<VkExtensionProperties> extensions(extention_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &extention_count, extensions.data());

        // display extension data
        std::cout << "availible extensions:\n";

        for (const auto& extension : extensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }
        std::cout << std::flush;

        // add extension info appInfofor the Vulkan instance 
        create_info.enabledExtensionCount = glfw_extesion_count;
        create_info.ppEnabledExtensionNames = glfw_extensions;
        // more about validation layers later; for now just plug a zero
        create_info.enabledLayerCount = 0;

        /* Create the Vulkan instance */
        // NOTE: custom allocator callbacks will not be used throughout
        //       this tutorial, and for this reason the second argument
        //       will always be nullptr.
        if (vkCreateInstance(&create_info, nullptr, &_instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create a Vulkan instance!");
        }

        /*  */
    }

    void init_vulkan() {
        create_instance();
    }

    void main_loop() {
        while (!glfwWindowShouldClose(_window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    // Private members
    VkInstance _instance;

    GLFWwindow* _window;
    const uint32_t _width = 800;
    const uint32_t _hight = 600;
};

int main() {
    hello_triangle_application app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
