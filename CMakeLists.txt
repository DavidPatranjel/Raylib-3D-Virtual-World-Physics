cmake_minimum_required(VERSION 3.10)
project(3DVirtualWorldPhysics CXX)
set(CMAKE_CXX_STANDARD 17)

# Try to find Raylib (if already installed on system)
find_package(raylib 4.5 QUIET)

# If not found, download and build automatically
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

# Your executable target
add_executable(mmo-virtual-world
        src/main.cpp
        src/Camera3DController.cpp
        src/Camera3DController.h
        src/SceneManager.cpp
        src/SceneManager.h
        src/PhysicsObject.cpp
        src/PhysicsObject.h
        src/ObjectManager.cpp
        src/ObjectManager.h
        src/UIManager.cpp
        src/UIManager.h
)

# Link Raylib
target_link_libraries(mmo-virtual-world raylib)

# Platform-specific settings
if (WIN32)
    target_link_libraries(mmo-virtual-world winmm)
endif()

# Copy resources to build directory (add this when you have assets)
# file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})