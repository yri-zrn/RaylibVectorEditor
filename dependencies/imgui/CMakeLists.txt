cmake_minimum_required(VERSION 3.5)
project(imgui)

add_library(imgui
    STATIC
        imconfig.h
        imgui.cpp
        imgui.h
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_internal.h
        imgui_widgets.cpp
        imgui_tables.cpp
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h
		
		imgui_spectrum.cpp
		imgui_spectrum.h
		
		plugins/ImGuizmo/ImGuizmo.h
		plugins/ImGuizmo/ImGuizmo.cpp
		
		backends/raylib/rlImGui.h
		backends/raylib/rlImGui.cpp
		backends/raylib/rlImGuiColors.h
		backends/imgui_impl_raylib.h
    )

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/backends/raylib)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/plugins/ImGuizmo)