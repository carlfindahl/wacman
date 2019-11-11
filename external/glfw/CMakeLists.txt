# Set up GLFW, either by finding the package directly, or by fetching it from GitHub

include(FetchContent)
message(STATUS "Getting GLFW from GitHub.")

# Declare where to find glfw and what version to use
FetchContent_Declare(
    glfw_external
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG b0796109629931b6fa6e449c15a177845256a407
    GIT_PROGRESS TRUE
)

# Populate and make available
FetchContent_MakeAvailable(glfw_external)

