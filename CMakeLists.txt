cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Chip8Emulator VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable and source files
add_executable(emulator main.cpp GraphicsHandler.cpp)

# Include directories (if needed)
target_include_directories(emulator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link libraries
find_package(raylib REQUIRED) # Assumes raylib is installed and discoverable

# Add platform-specific libraries
if(APPLE)
    target_link_libraries(emulator raylib m pthread dl "-framework OpenGL" "-framework Cocoa" "-framework IOKit")
elseif(UNIX)
    target_link_libraries(emulator raylib m pthread dl)
endif()
