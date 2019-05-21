# Set up GLFW, either by finding the package directly, or by fetching it from GitHub

find_package(glfw3 3.3 REQUIRED)

# If we found it, then propagate it globally so we can see it elsewhere
if(glfw3_FOUND)
    set_target_properties(
        glfw
        PROPERTIES
        IMPORTED_GLOBAL TRUE
    )
endif()
