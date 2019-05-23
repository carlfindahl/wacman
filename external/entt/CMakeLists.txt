# Set up EnTT, either by finding the package directly, or by fetching it from GitHub

find_package(EnTT 3.0)

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
        GIT_TAG 85152bac3462c95e7be02cd4077bef8fc56de804
        GIT_PROGRESS TRUE
    )

    # Populate it for building
    if(NOT EnTT_external_POPULATED)
        FetchContent_Populate(EnTT_external)
        add_subdirectory(${EnTT_external_SOURCE_DIR} ${EnTT_external_BINARY_DIR})
    endif()
endif()
