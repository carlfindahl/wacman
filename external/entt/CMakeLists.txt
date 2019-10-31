# Set up EnTT, either by finding the package directly, or by fetching it from GitHub
include(FetchContent)
message(STATUS "Fetching EnTT from GitHub using FetchContent.")

# Declare where to find glfw and what version to use
FetchContent_Declare(
    EnTT_external
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.2.0
    GIT_PROGRESS TRUE
)

# Populate and make available
FetchContent_MakeAvailable(EnTT_external)
