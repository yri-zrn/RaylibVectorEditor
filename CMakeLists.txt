cmake_minimum_required(VERSION 3.12)
project(course_work)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    course_work
        src/main.cpp

        src/Core.hpp
        src/RaylibObject.hpp
        
        src/RaylibApplication.hpp
        src/RaylibApplication.cpp
        src/Environment.hpp
        src/Environment.cpp

        src/Scene.hpp
        src/Scene.cpp
        src/SceneRenderer.hpp
        src/SceneRenderer.cpp
        src/SceneSerializer.hpp
        src/SceneSerializer.cpp
        src/Entity.hpp
        src/Entity.cpp

        src/ViewportCamera.hpp
        src/ViewportCamera.cpp
        src/ViewportCollisionResolver.cpp
        src/ViewportCollisionResolver.hpp
        
        src/Tool.hpp
        src/Tool.cpp

        src/Panel.hpp
        src/Panel.cpp
        src/ViewportWindow.hpp
        src/ViewportWindow.cpp
        src/HierarchyWindow.hpp
        src/HierarchyWindow.cpp
        src/PropertiesWindow.hpp
        src/PropertiesWindow.cpp
    )

add_subdirectory(dependencies)

target_include_directories(course_work PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/assets)

find_package(raylib REQUIRED)
find_package(cereal REQUIRED)

target_link_libraries(
    course_work
        PRIVATE
        raylib
        entt
        imgui
        cereal::cereal
        nfd
    )