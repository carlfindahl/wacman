# This file is used to add any External Projects to the project so they
# download and build automatically if needed. Needed means: Not found 
# with find_package on the given system.

include(FetchContent)

# First add GLFW
message(STATUS "Adding GLFW as External Project")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 999f3556fdd80983b10051746264489f2cb1ef16
)

# Then Add GLM
message(STATUS "Adding GLM as External Project")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 9749727c2db4742369219e1d452f43e918734b4e
)

# Finally OpenAL Soft
message(STATUS "Adding OpenAL as External Project")
FetchContent_Declare(
    openal
    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
    GIT_TAG 6761218e51699f46bf25c377e65b3e9ea5e434b9
)
