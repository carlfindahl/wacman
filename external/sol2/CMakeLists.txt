# Set up sol2, either by finding the package directly, or by fetching it from GitHub

include(FetchContent)
message(STATUS "Getting sol2 from GitHub.")

# Declare where to find glfw and what version to use
FetchContent_Declare(
    sol2_external
    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
    GIT_TAG v3.0.3
    GIT_PROGRESS TRUE
)

# Populate it for building
FetchContent_MakeAvailable(sol2_external)

