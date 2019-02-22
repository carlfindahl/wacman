
if (${PACMAN_BUILD_BENCHMARKS})
    find_package(benchmark 1.4)

    target_sources(
        ${EXEC_NAME}
        PRIVATE

        ${CMAKE_CURRENT_LIST_DIR}/benchmarks.cpp
    )
endif()