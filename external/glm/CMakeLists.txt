# Set up GLM, either by finding the package directly, or by fetching it from GitHub

find_package(glm)

# If we found it, then propagate it globally so we can see it elsewhere
if(glm_FOUND)
    set_target_properties(
        glm
        PROPERTIES
        IMPORTED_GLOBAL TRUE
    )

# If we could not find it then add it locally
else()
    include(FetchContent)
    message(STATUS "Could not find GLM package so we are getting it from GitHub instead.")

    # Declare where to find glfw and what version to use
    FetchContent_Declare(
        glm_external
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 9749727c2db4742369219e1d452f43e918734b4e
        GIT_PROGRESS TRUE
    )

    # Populate it for building
    if(NOT glm_external_POPULATED)
        FetchContent_Populate(glm_external)
        add_subdirectory(${glm_external_SOURCE_DIR} ${glm_external_BINARY_DIR})
    endif()
endif()
