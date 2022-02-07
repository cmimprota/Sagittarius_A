cmake_minimum_required(VERSION 2.8)
 
PROJECT(VisProject)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
add_executable(VisProject project.cpp)
 
target_link_libraries(VisProject ${VTK_LIBRARIES})