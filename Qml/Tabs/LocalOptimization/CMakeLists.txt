project(LocalOptimization VERSION 0.1 LANGUAGES CXX)

qt_add_qml_module(${PROJECT_NAME}
    STATIC
    URI ${PROJECT_NAME}
    QML_FILES
       InputParams.qml
       ReportTab.qml
       TaskTab.qml
)
