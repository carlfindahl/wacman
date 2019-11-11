# Set up GLM, either by finding the package directly, or by fetching it from GitHub

include(FetchContent)
message(STATUS "Get GLM package from GitHub.")

# Declare where to find glfw and what version to use
FetchContent_Declare(
    glm_external
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 9749727c2db4742369219e1d452f43e918734b4e
    GIT_PROGRESS TRUE
)

# Populate it for building
FetchContent_MakeAvailable(glm_external)
