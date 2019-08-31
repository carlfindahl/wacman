# Set up GLFW, either by finding the package directly, or by fetching it from GitHub

find_package(glfw3 3.3 REQUIRED)

# If we found it, then propagate it globally so we can see it elsewhere
if(glfw3_FOUND)
    set_target_properties(
        glfw
        PROPERTIES
        IMPORTED_GLOBAL TRUE
    )
else()
    include(FetchContent)
    message(STATUS "Could not find GLFW package so we are getting it from GitHub instead.")

    # Declare where to find glfw and what version to use
    FetchContent_Declare(
        glfw_external
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG b0796109629931b6fa6e449c15a177845256a407
        GIT_PROGRESS TRUE
    )

    # Populate and make available
    FetchContent_MakeAvailable(glfw_external)
endif()
