#include <hello_triangle_app.hpp>

/**
 * A quick side note on code styling: I prefer to use
 * snake-case instead of camelback-case. It is consistent
 * with the style of the standard library and I consider it
 * to be more aesthetically pleasing. As a bonus, it allows
 * to better differenciate between API calls and my own code.
 */


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
