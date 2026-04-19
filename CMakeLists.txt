cmake_minimum_required(VERSION 4.0)
project(GD2_CPP_CA3)

set(CMAKE_CXX_STANDARD 17)

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
        include/Hunter.h)

target_include_directories(GD2_CPP_CA3 PRIVATE include)

# Copy the whole data folder next to the built executable after each build
add_custom_command(TARGET GD2_CPP_CA3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:GD2_CPP_CA3>/data
)