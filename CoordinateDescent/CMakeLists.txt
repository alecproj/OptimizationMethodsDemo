cmake_minimum_required(VERSION 3.16)

project(CoordinateDescent VERSION 0.1 LANGUAGES CXX)

# Создаёт цель CoordinateDescent.
# - Если в папке есть .cpp (кроме main.cpp) — создаётся STATIC library.
# - Если .cpp отсутствуют (header-only) — создаётся INTERFACE library.
# - Если есть main.cpp и опция BUILD_STANDALONE=ON — создаётся исполняемый файл.

option(COORDINATEDESCENT_BUILD_STANDALONE "Build CoordinateDescent standalone exe" ${COORDINATEDESCENT_BUILD_STANDALONE})

file(GLOB_RECURSE CD_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE CD_SOURCES_ALL CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx" "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

# отделяем main.cpp (если он есть) — использовать для standalone exe
set(CD_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
list(REMOVE_ITEM CD_SOURCES_ALL ${CD_MAIN})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ExprTk QUIET)

if(ExprTk_FOUND)
    message(STATUS "ExprTk found by find_package")
    set(EXPRTK_TARGET exprtk::exprtk)
else()
    # Попытка 2: если top-level уже добавил ExprTk, то root передал CACHE переменную
    if(EXPRTK_FROM_TOPLEVEL)
        message(STATUS "ExprTk provided by top-level (add_subdirectory)")
        set(EXPRTK_TARGET exprtk::exprtk) # таргет объявлен top-level
    endif()
endif()

# Если после удаления main.cpp не осталось cpp — значит header-only
if(CD_SOURCES_ALL)
    add_library(CoordinateDescent STATIC ${CD_SOURCES_ALL} ${CD_HEADERS})
else()
    add_library(CoordinateDescent INTERFACE)
endif()

# #include <CoordinateDescent/Common.hpp>
target_include_directories(CoordinateDescent
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_compile_features(CoordinateDescent INTERFACE cxx_std_20)

# Подключаем ExprTk к библиотеке (если он доступен)
if(TARGET ${EXPRTK_TARGET})
    target_link_libraries(CoordinateDescent PUBLIC ${EXPRTK_TARGET})
else()
    message(WARNING "ExprTk target is not available; CoordinateDescent will be built without ExprTk")
endif()

# Standalone exe (использует main.cpp в этой папке)
if(COORDINATEDESCENT_BUILD_STANDALONE AND EXISTS ${CD_MAIN})
    add_executable(CoordinateDescent_app ${CD_MAIN})
    target_link_libraries(CoordinateDescent_app PRIVATE CoordinateDescent)
    target_link_libraries(CoordinateDescent_app INTERFACE exprtk)
    # при желании - добавить RUNTIME_OUTPUT_DIRECTORY и т.д.
endif()
