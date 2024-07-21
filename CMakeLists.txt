cmake_minimum_required(VERSION 3.10)
project(ChessEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-sign-compare -O2")

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/*.hh")

# SDL2 dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2 SDL2_image SDL2_mixer)

# Include SDL2 directories
include_directories(${SDL2_INCLUDE_DIRS})

# Add executable
add_executable(engine ${SRC_FILES} ${HEADER_FILES})

# Link SDL2 libraries
target_link_libraries(engine ${SDL2_LIBRARIES})

# Set output directories to build directory
set_target_properties(engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME engine  # Rename executable to 'engine'
)

# Debug target
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_BUILD_TYPE Release)

# Add custom target for distclean
add_custom_target(distclean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Removing build directory"
)

# Target-specific include directories
target_include_directories(engine PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Ensure proper linking of SDL2
target_link_libraries(engine ${SDL2_LIBRARIES})
