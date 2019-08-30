# Set up EnTT, either by finding the package directly, or by fetching it from GitHub

find_package(EnTT 3.1)

# If we found it, then propagate it globally so we can see it elsewhere
if(EnTT_FOUND)
    set_target_properties(
        EnTT
        PROPERTIES
        IMPORTED_GLOBAL TRUE
    )

# If we could not find it then add it locally
else()
    include(FetchContent)
    message(STATUS "Could not find EnTT package so we are getting it from GitHub instead.")

    # Declare where to find glfw and what version to use
    FetchContent_Declare(
        EnTT_external
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.1.0
        GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(EnTT_external)

endif()
