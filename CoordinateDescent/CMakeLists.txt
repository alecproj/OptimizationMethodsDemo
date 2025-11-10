cmake_minimum_required(VERSION 3.16)

project(CoordinateDescent VERSION 0.1 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# -------------------- muparser -----------------------
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(MUPARSER_TARGET_NAME muparser CACHE STRING "Target name created by muparser CMake")
set(MUPARSER_DIR "${CMAKE_CURRENT_LIST_DIR}/../muparser" CACHE PATH "Path to muparser sources or submodule")

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

# Если после удаления main.cpp не осталось cpp — значит header-only
if(CD_SOURCES_ALL)
    add_library(CoordinateDescent STATIC ${CD_SOURCES_ALL} ${CD_HEADERS})
else()
    add_library(CoordinateDescent INTERFACE)
endif()

target_link_libraries(CoordinateDescent PRIVATE ${MUPARSER_TARGET_NAME})

# #include <CoordinateDescent/Common.hpp>
target_include_directories(CoordinateDescent
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_compile_features(CoordinateDescent INTERFACE cxx_std_20)

# Standalone exe (использует main.cpp в этой папке)
if(COORDINATEDESCENT_BUILD_STANDALONE AND EXISTS ${CD_MAIN})
    add_executable(CoordinateDescent_app ${CD_MAIN})
    target_link_libraries(CoordinateDescent_app
        PRIVATE 
            ${MUPARSER_TARGET_NAME}
            CoordinateDescent
    )
    # при желании - добавить RUNTIME_OUTPUT_DIRECTORY и т.д.
endif()
