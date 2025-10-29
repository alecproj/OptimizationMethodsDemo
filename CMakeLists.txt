cmake_minimum_required(VERSION 3.16)

project(OptimizationMethodsDemo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Если вы добавляете репозиторий как submodule в другой проект,
# можно переопределить эти опции извне.
option(COORDINATEDESCENT_BUILD_STANDALONE "Build CoordinateDescent as standalone exe" OFF)
option(GRADIENTDESCENT_BUILD_STANDALONE "Build GradientDescent as standalone exe" OFF)

# --------------- Qt ---------------

find_package(Qt6 REQUIRED COMPONENTS Quick Core)
qt_standard_project_setup(REQUIRES 6.8)

# --------------- ExprTk --------------------

option(USE_TOPLEVEL_EXPRTK "Use ExprTk included in top-level source tree" ON)

if(USE_TOPLEVEL_EXPRTK AND EXISTS "${CMAKE_SOURCE_DIR}/ExprTk/CMakeLists.txt")
    message(STATUS "Adding top-level ExprTk")
    add_subdirectory("${CMAKE_SOURCE_DIR}/ExprTk" EXCLUDE_FROM_ALL) # добавляем, но можно EXCLUDE_FROM_ALL если не хотим собирать тесты
    # пометим, что ExprTk доступен (PROPAGATE)
    set(EXPRTK_FROM_TOPLEVEL TRUE CACHE BOOL "ExprTk provided from top-level" FORCE)
else()
    set(EXPRTK_FROM_TOPLEVEL FALSE CACHE BOOL "ExprTk provided from top-level" FORCE)
endif()

# --------------- Subprojects ---------------

# add_subdirectory(ExprTk)
add_subdirectory(CoordinateDescent)
add_subdirectory(GradientDescent)

add_subdirectory(Sources)

# --------------- Application (Qt) ---------------

qt_add_executable(${PROJECT_NAME}App
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
