project(Globals VERSION 0.1 LANGUAGES CXX)

set_source_files_properties(AppStates.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
set_source_files_properties(AppPalette.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(${PROJECT_NAME}
    STATIC
    URI ${PROJECT_NAME} 
    QML_FILES
        AppStates.qml
        AppPalette.qml
)

