cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE APP_HEADERS_REL
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

file(GLOB_RECURSE APP_SOURCES_ALL
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
)

# Преобразуем относительные пути заголовков в абсолютные директории и удалим дубли
set(APP_HEADER_DIRS "")
foreach(h ${APP_HEADERS_REL})
    get_filename_component(dir "${h}" DIRECTORY)   # относительная директория или "" для корня
    if(NOT dir STREQUAL "")
        # Записываем абсолютный путь к директории
        list(APPEND APP_HEADER_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
    endif()
endforeach()
list(REMOVE_DUPLICATES APP_HEADER_DIRS)

set(APP_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
list(REMOVE_ITEM APP_SOURCES_ALL ${APP_MAIN})

if(APP_SOURCES_ALL)
    add_library(app_sources STATIC ${APP_SOURCES_ALL} ${APP_HEADERS_REL})
else()
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/app_sources_dummy.cpp" "// dummy source for app_sources\n")
    add_library(app_sources STATIC
        ${APP_HEADERS_REL}
        "${CMAKE_CURRENT_BINARY_DIR}/app_sources_dummy.cpp"
    )
endif()
target_include_directories(app_sources
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        ${APP_HEADER_DIRS}
)

set_target_properties(app_sources PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

find_package(Qt6 REQUIRED COMPONENTS Core Core5Compat)
target_link_libraries(app_sources
    PUBLIC
        Qt6::Core
        Qt6::Core5Compat
        CoordinateDescent
        GradientDescent
        ConjugateGradient
)
