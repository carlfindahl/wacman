# Set up sol2, either by finding the package directly, or by fetching it from GitHub

find_package(sol2 3.0.3)

# If we found it, then propagate it globally so we can see it elsewhere
if(sol2_FOUND)
    set_target_properties(
        sol2::sol2
        PROPERTIES
        IMPORTED_GLOBAL TRUE
    )

# If we could not find it then add it locally
else()
    include(FetchContent)
    message(STATUS "Could not find sol2 package so we are getting it from GitHub instead.")

    # Declare where to find glfw and what version to use
    FetchContent_Declare(
        sol2_external
        GIT_REPOSITORY https://github.com/ThePhD/sol2.git
        GIT_TAG v3.0.3
        GIT_PROGRESS TRUE
    )

    # Populate it for building
    FetchContent_MakeAvailable(sol2_external)

endif()
