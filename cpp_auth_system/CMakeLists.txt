cmake_minimum_required(VERSION 3.10)
project(SecureAuthSystem VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/SecurityConfig.cpp
    src/PasswordHasher.cpp
    src/TwoFactorAuth.cpp
    src/Encryption.cpp
    src/User.cpp
    src/AuthenticationManager.cpp
)

# Create library
add_library(auth_system STATIC ${SOURCES})

# Example executable
add_executable(auth_demo examples/main.cpp)
target_link_libraries(auth_demo auth_system)

# Installation
install(TARGETS auth_system auth_demo
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include/auth_system
    FILES_MATCHING PATTERN "*.h"
)

# Compiler warnings
if(MSVC)
    target_compile_options(auth_system PRIVATE /W4)
else()
    target_compile_options(auth_system PRIVATE -Wall -Wextra -pedantic)
endif()

# Enable testing (optional)
enable_testing()

# Print build information
message(STATUS "Building SecureAuthSystem v${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
