cmake_minimum_required(VERSION 3.16)

project(OptimizationMethodsDemo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Если вы добавляете репозиторий как submodule в другой проект,
# можно переопределить эти опции извне.
option(COORDINATEDESCENT_BUILD_STANDALONE "Build CoordinateDescent as standalone exe" OFF)
option(GRADIENTDESCENT_BUILD_STANDALONE "Build GradientDescent as standalone exe" OFF)

# ----------- QML Files ------------

file(GLOB_RECURSE QML_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Qml/*.qml")

# --------------- Qt ---------------

find_package(Qt6 REQUIRED COMPONENTS Quick Core)
qt_standard_project_setup(REQUIRES 6.8)

# ------------ muparser ------------
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(MUPARSER_TARGET_NAME muparser CACHE STRING "Target name created by muparser CMake")
set(MUPARSER_DIR "${CMAKE_CURRENT_LIST_DIR}/muparser" CACHE PATH "Path to muparser sources or submodule")

if (NOT TARGET ${MUPARSER_TARGET_NAME})
  if (EXISTS "${MUPARSER_DIR}/CMakeLists.txt")
    message(STATUS "Using muparser from ${MUPARSER_DIR}")
    add_subdirectory("${MUPARSER_DIR}" "${CMAKE_BINARY_DIR}/third_party/muparser" EXCLUDE_FROM_ALL)
  else()
    message(STATUS "muparser not found at ${MUPARSER_DIR}, falling back to FetchContent")
    include(FetchContent)
    FetchContent_Declare(
      muparser
      GIT_REPOSITORY https://github.com/beltoforion/muparser.git
      GIT_TAG v2.3.5
    )
    FetchContent_MakeAvailable(muparser)
  endif()
endif()

# --------------- Subprojects ---------------

add_subdirectory(CoordinateDescent)
add_subdirectory(GradientDescent)

add_subdirectory(Sources)

# --------------- Application (Qt) ---------------
set(CMAKE_AUTORCC ON)

qt_add_executable(${PROJECT_NAME}App
    resources.qrc
    Sources/main.cpp
)

qt_add_qml_module(${PROJECT_NAME}App
    URI ${PROJECT_NAME}
    VERSION 1.0
    QML_FILES
        Qml/Main.qml
)

target_link_libraries(${PROJECT_NAME}App
    PRIVATE
        app_sources
        ${MUPARSER_TARGET_NAME}
        CoordinateDescent
        GradientDescent
        Qt6::Quick
)

set_target_properties(${PROJECT_NAME}App PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}App
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
