cmake_minimum_required(VERSION 3.10)

# Project name and version
project(TestProject VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(emulator main.cpp)

# Link libraries
find_package(raylib REQUIRED) # Assumes raylib is installed and discoverable

# Add platform-specific libraries
if(APPLE)
    target_link_libraries(emulator raylib m pthread dl "-framework OpenGL" "-framework Cocoa" "-framework IOKit")
elseif(UNIX)
    target_link_libraries(emulator raylib m pthread dl)
endif()