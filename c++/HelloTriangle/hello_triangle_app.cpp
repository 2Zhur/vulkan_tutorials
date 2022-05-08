#include <hello_triangle_app.hpp>


VkResult create_debug_utils_messenger(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* create_info,
    const VkAllocationCallbacks* allocator,
    VkDebugUtilsMessengerEXT* debug_messenger) {
    
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        return func(instance, create_info, allocator, debug_messenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroy_debug_utils_messenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks* allocator) {
    
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debug_messenger, allocator);
    }
}

queue_family_indices hello_triangle_application::find_queue_families(VkPhysicalDevice device) {
    queue_family_indices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());


    int i = 0;
    for (const auto& queue_family : queue_families) {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }
        
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if (present_support) {
            indices.present_family = i;
        }
        if (indices.is_complete()) {
            break;
        }
        i++;
    }

    return indices;
}

void hello_triangle_application::run() {
    init_window();
    init_vulkan();
    main_loop();
    cleanup();
}

void hello_triangle_application::init_window() {
    glfwInit();
    // tell GLFW not to create an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // disable window resizing for now
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // create the window
    window = glfwCreateWindow(width, hight, "Vulkan", nullptr, nullptr);
}

bool hello_triangle_application::check_validation_layer_support() {
    // gather info about available validation layers
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
    // compare the acquired list of available validation
    // layer to the list of requested ones
    for (const char* layer_name : validation_layers) {
        bool layer_found = false;
        for (const auto& layer_properties : available_layers) {
            if (strcmp(layer_name, layer_properties.layerName) == 0) {
                layer_found = true;
                break;
            }
        }
        if (!layer_found) {
            return false;
        }
    }
    return true;
}

void hello_triangle_application::setup_debug_messenger() {
    if (!enable_validation_layers) return;

    VkDebugUtilsMessengerCreateInfoEXT create_info;
    populate_debug_messenger_create_info(create_info);
    if (create_debug_utils_messenger(instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void hello_triangle_application::create_surface() {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void hello_triangle_application::populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT& create_info) {

    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
    // create_info.pUserData = nullptr; // optional
}

void hello_triangle_application::pick_physical_device() {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    if (device_count == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    auto is_device_suitable = [&] (VkPhysicalDevice device) {
        return find_queue_families(device).is_complete();
    };

    for (const auto& device : devices) {
        if (is_device_suitable(device)) {
            physical_device = device;
            break;
        }
    }
    if (physical_device == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

std::vector<const char*> hello_triangle_application::get_required_extensions() {
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
    if (enable_validation_layers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void hello_triangle_application::create_instance() {
    if (enable_validation_layers && !check_validation_layer_support()) {
        throw std::runtime_error("some of the requested validation layers are unavailable!");
    }
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
    // add extension info for the Vulkan instance 
    auto glfw_extensions = get_required_extensions();
    create_info.enabledExtensionCount = static_cast<uint32_t>(glfw_extensions.size());
    create_info.ppEnabledExtensionNames = glfw_extensions.data();
    // enable requested validation layers, if any
    if (enable_validation_layers) {
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
        create_info.enabledLayerCount = 0;
    }

    /* Add debug functionality */
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
    if (enable_validation_layers) {
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
        populate_debug_messenger_create_info(debug_create_info);
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
    } else {
        create_info.enabledLayerCount = 0;
        create_info.pNext = nullptr;
    }
    
    /* Create the Vulkan instance */
    // NOTE: custom allocator callbacks will not be used throughout
    //       this tutorial, and for this reason the second argument
    //       will always be nullptr.
    if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create a Vulkan instance!");
    }

}

void hello_triangle_application::create_logical_device() {
    queue_family_indices indices;

    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = indices.graphics_family.value();
    queue_create_info.queueCount = 1;
    float queue_priority = 1.0f;
    queue_create_info.pQueuePriorities = &queue_priority;

    VkPhysicalDeviceFeatures device_features{}; // empty for now

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = &queue_create_info;
    create_info.queueCreateInfoCount = 1;
    create_info.pEnabledFeatures = &device_features;

    create_info.enabledExtensionCount = 0;
    if (enable_validation_layers) {
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
        create_info.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physical_device, &create_info, nullptr, &logical_device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device");
    }

    vkGetDeviceQueue(logical_device, indices.graphics_family.value(), 0, &graphics_queue);
}

void hello_triangle_application::init_vulkan() {
    create_instance();
    setup_debug_messenger();
    create_surface();
    pick_physical_device();
    create_logical_device();
}

void hello_triangle_application::main_loop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void hello_triangle_application::cleanup() {
    if (enable_validation_layers) {
        destroy_debug_utils_messenger(instance, debug_messenger, nullptr);
    }
    vkDestroyDevice(logical_device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}