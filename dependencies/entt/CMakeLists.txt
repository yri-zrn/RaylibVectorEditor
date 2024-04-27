cmake_minimum_required(VERSION 3.5)
project(entt)

add_library(entt
    INTERFACE
    	include/entt.hpp
    )

target_include_directories(entt INTERFACE include)