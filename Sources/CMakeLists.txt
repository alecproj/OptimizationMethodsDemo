cmake_minimum_required(VERSION 3.16)

# Собираем заголовки и .cpp в Sources (кроме main.cpp — он в корне).
file(GLOB APP_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB APP_SOURCES_ALL CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx" "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

set(APP_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
list(REMOVE_ITEM APP_SOURCES_ALL ${APP_MAIN})

# Всегда создаём статическую библиотеку app_sources.
# Если в папке нет .cpp (кроме main.cpp), добавим заголовки как source + dummy cpp,
# чтобы CMake мог запустить AUTOMOC и корректно сгенерировать moc-цели.
if(APP_SOURCES_ALL)
    add_library(app_sources STATIC ${APP_SOURCES_ALL} ${APP_HEADERS})
else()
    # добавляем заголовки в target sources, и создаём "физический" пустой cpp в build dir
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/app_sources_dummy.cpp" "// dummy source for app_sources\n")
    add_library(app_sources STATIC
        ${APP_HEADERS}
        "${CMAKE_CURRENT_BINARY_DIR}/app_sources_dummy.cpp"
    )
endif()

# #include "Reporter.hpp"
target_include_directories(app_sources
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Автоматический запуск moc/uic/рc на этой цели
set_target_properties(app_sources PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# Нужна линковка с QtCore (и т.д.), чтобы moc-файлы компилировались с правильными путями.
# Используем PUBLIC чтобы include dirs и прочее пропагировалось в исполняемый target при линке.
find_package(Qt6 REQUIRED COMPONENTS Core)  # если уже найдено в корне — это быстро вернёт результат
target_link_libraries(app_sources
    PUBLIC 
        Qt6::Core
        CoordinateDescent
        GradientDescent
)
