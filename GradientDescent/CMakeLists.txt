cmake_minimum_required(VERSION 3.16)

project(GradientDescent VERSION 0.1 LANGUAGES CXX)

# Создаёт цель GradientDescent.
# - Если в папке есть .cpp (кроме main.cpp) — создаётся STATIC library.
# - Если .cpp отсутствуют (header-only) — создаётся INTERFACE library.
# - Если есть main.cpp и опция BUILD_STANDALONE=ON — создаётся исполняемый файл.

option(GRADIENTDESCENT_BUILD_STANDALONE "Build GradientDescent standalone exe" ${GRADIENTDESCENT_BUILD_STANDALONE})

file(GLOB_RECURSE GD_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE GD_SOURCES_ALL CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx" "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

set(GD_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
list(REMOVE_ITEM GD_SOURCES_ALL ${GD_MAIN})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(GD_SOURCES_ALL)
    add_library(GradientDescent STATIC ${GD_SOURCES_ALL} ${GD_HEADERS})
else()
    add_library(GradientDescent INTERFACE)
endif()

target_include_directories(GradientDescent
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_compile_features(GradientDescent INTERFACE cxx_std_17)

if(GRADIENTDESCENT_BUILD_STANDALONE AND EXISTS ${GD_MAIN})
    add_executable(GradientDescent_app ${GD_MAIN})
    target_link_libraries(GradientDescent_app PRIVATE GradientDescent)
endif()
