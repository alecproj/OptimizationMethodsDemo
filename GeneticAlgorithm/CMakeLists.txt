cmake_minimum_required(VERSION 3.16)

project(GeneticAlgorithm VERSION 0.1 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------- glog -------------------------
find_package (glog 0.7.1 REQUIRED)

# ---------------- GeneticAlgorithm ------------------

# Создаёт цель GeneticAlgorithm.
# - Если в папке есть .cpp (кроме main.cpp) — создаётся STATIC library.
# - Если .cpp отсутствуют (header-only) — создаётся INTERFACE library.
# - Если есть main.cpp и опция BUILD_STANDALONE=ON — создаётся исполняемый файл.

option(GENETIC_BUILD_STANDALONE "Build GeneticAlgorithm standalone exe" ${GENETIC_BUILD_STANDALONE})

file(GLOB_RECURSE GD_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE GD_SOURCES_ALL CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx" "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

set(GD_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
list(REMOVE_ITEM GD_SOURCES_ALL ${GD_MAIN})

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

if(GD_SOURCES_ALL)
    add_library(${PROJECT_NAME} STATIC ${GD_SOURCES_ALL} ${GD_HEADERS} "GeneticEncoder.cpp")
else()
    add_library(${PROJECT_NAME} INTERFACE)
endif()

target_link_libraries(${PROJECT_NAME}
    PUBLIC 
        ${MUPARSER_TARGET_NAME}
        glog::glog
)

target_include_directories(${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

if(GENETIC_BUILD_STANDALONE AND EXISTS ${GD_MAIN})
    add_executable(${PROJECT_NAME}App ${GD_MAIN} "GeneticEncoder.cpp")
    target_link_libraries(${PROJECT_NAME}App
        PRIVATE
            ${MUPARSER_TARGET_NAME}
            ${PROJECT_NAME}
    )
endif()
