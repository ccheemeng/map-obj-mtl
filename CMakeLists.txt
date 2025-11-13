cmake_minimum_required(VERSION 3.12...3.29)
project(map_obj_mtl)

find_package(CGAL REQUIRED)

add_executable(map_obj_mtl map_obj_mtl.cpp)
target_link_libraries(map_obj_mtl CGAL::CGAL)
