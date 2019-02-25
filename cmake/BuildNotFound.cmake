# If not found, build dependencies locally

include(FetchContent)

if(NOT glfw3_FOUND)
    FetchContent_GetProperties(glfw)
    if(NOT glfw_POPULATED)
        FetchContent_Populate(glfw)
        add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
    endif()
endif()

if(NOT glm_FOUND)
    FetchContent_GetProperties(glm)
    if(NOT glm_POPULATED)
        FetchContent_Populate(glm)
        add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
    endif()
endif()

if(NOT OPENAL_FOUND)
    FetchContent_GetProperties(openal)
    if(NOT openal_POPULATED)
        FetchContent_Populate(openal)
        set(ALSOFT_EXAMPLES OFF)
        set(ALSOFT_INSTALL ON)
        add_subdirectory(${openal_SOURCE_DIR} ${openal_BINARY_DIR})
    endif()
endif()
