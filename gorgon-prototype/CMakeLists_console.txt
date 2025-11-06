cmake_minimum_required(VERSION 3.15)
project(GorgonPrototypeTest VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Collect source files (excluding main.cpp, using test_main.cpp instead)
set(SOURCES
    src/test_main.cpp
    src/Character.cpp
    src/Skill.cpp
    src/Ability.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Platform-specific settings
if(UNIX AND NOT APPLE)
    # Linux-specific settings
    target_link_libraries(${PROJECT_NAME} PRIVATE m pthread)
endif()

# Print configuration
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Console-only test build (no Raylib required)")
