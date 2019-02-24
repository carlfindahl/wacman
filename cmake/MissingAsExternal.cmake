# This file is used to add any External Projects to the project so they
# download and build automatically as needed

include(ExternalProject)

# First add GLFW
message(STATUS "Adding GLFW as External Project")
ExternalProject_Add(
    glfw_ext
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 999f3556fdd80983b10051746264489f2cb1ef16
    CMAKE_ARGS -D CMAKE_BUILD_TYPE=Release
               -D CMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

# Then Add GLM
message(STATUS "Adding GLM as External Project")
ExternalProject_Add(
    glm_ext
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 9749727c2db4742369219e1d452f43e918734b4e
    CMAKE_ARGS -D CMAKE_BUILD_TYPE=Release
               -D CMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               -D GLM_TEST_ENABLE_CXX_17=ON
)

# Finally OpenAL Soft
message(STATUS "Adding OpenAL as External Project")
ExternalProject_Add(
    openal_ext
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/openal
    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
    GIT_TAG 6761218e51699f46bf25c377e65b3e9ea5e434b9
    CMAKE_ARGS -D CMAKE_BUILD_TYPE=Release
               -D CMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               -D ALSOFT_EXAMPLES=OFF
)
