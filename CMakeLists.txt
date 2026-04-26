cmake_minimum_required(VERSION 4.0)
project(GD2_CPP_CA3)

set(CMAKE_CXX_STANDARD 17)

# =========================
# Compile profiling options
# =========================
option(ENABLE_GCC_TIME_REPORT "Enable GCC compile-time phase report using -ftime-report" OFF)
option(ENABLE_GCC_TIME_REPORT_DETAILS "Enable detailed GCC compile-time report using -ftime-report-details" OFF)
option(ENABLE_GCC_TIME_TRACE "Enable GCC JSON compile-time tracing using -ftime-trace" OFF)
option(ENABLE_GCC_INCLUDE_TREE "Show header include tree using -H" OFF)

add_executable(GD2_CPP_CA3 main.cpp
        include/Bug.h
        src/Bug.cpp
        include/Crawler.h
        src/Crawler.cpp
        include/Hopper.h
        src/Hopper.cpp
        include/Board.h
        src/Board.cpp
        include/Seeder.h
        src/Seeder.cpp
        include/ConsoleRenderer.h
        src/ConsoleRenderer.cpp
        include/Utils.h
        src/Utils.cpp
        include/Hunter.h
        src/Hunter.cpp
        include/Terrain.h)

target_include_directories(GD2_CPP_CA3 PRIVATE include)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    if (ENABLE_GCC_TIME_REPORT)
        target_compile_options(GD2_CPP_CA3 PRIVATE -ftime-report)
    endif()

    if (ENABLE_GCC_TIME_REPORT_DETAILS)
        target_compile_options(GD2_CPP_CA3 PRIVATE -ftime-report-details)
    endif()

    if (ENABLE_GCC_TIME_TRACE)
        target_compile_options(GD2_CPP_CA3 PRIVATE -ftime-trace)
    endif()

    if (ENABLE_GCC_INCLUDE_TREE)
        target_compile_options(GD2_CPP_CA3 PRIVATE -H)
    endif()

else()
    message(STATUS "GCC compile profiling options are available only when using GCC.")
endif()

# Copy the whole data folder next to the built executable after each build
add_custom_command(TARGET GD2_CPP_CA3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:GD2_CPP_CA3>/data
)