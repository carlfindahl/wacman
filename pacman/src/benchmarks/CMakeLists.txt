# Add Google Benchmark and link it only if benchmark library is found and we have enabled them with the option

if (${PACMAN_BUILD_BENCHMARKS})
    find_package(benchmark)

    if(${benchmark_FOUND})
        target_link_libraries(
            ${EXEC_NAME}
            PRIVATE
            benchmark
        )

        target_sources(
            ${EXEC_NAME}
            PRIVATE

            ${CMAKE_CURRENT_LIST_DIR}/benchmarks.cpp
        )
    endif()
endif()
