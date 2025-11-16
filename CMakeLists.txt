cmake_minimum_required(VERSION 3.12...3.29)
project(MapOBJMaterial)

find_package(CGAL REQUIRED)

add_executable(map_obj_mtl source/map_obj_mtl.cpp)
target_link_libraries(map_obj_mtl CGAL::CGAL)

target_include_directories(map_obj_mtl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
