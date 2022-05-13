#include <vulkan_app.hpp>

void vulkan_app::run() {
    init_window();
    init_vulkan();
    main_loop();
    cleanup();
}

void vulkan_app::init_window() {
    glfwInit();
    // tell GLFW not to create an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // disable window resizing for now
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // create the window
    window = glfwCreateWindow(width, hight, "Vulkan", nullptr, nullptr);
}


void vulkan_app::init_vulkan() {
    create_instance();
    setup_debug_messenger();
    create_surface();
    pick_physical_device();
    create_logical_device();
}

void vulkan_app::main_loop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void vulkan_app::cleanup() {
    vkDestroySwapchainKHR(logical_device, swap_chain, nullptr); // SEGFAULT!
    if (enable_validation_layers) {
        destroy_debug_utils_messenger(instance, debug_messenger, nullptr);
    }
    vkDestroyDevice(logical_device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}