project(Components VERSION 0.1 LANGUAGES CXX)

qt_add_qml_module(${PROJECT_NAME}
    STATIC
    URI ${PROJECT_NAME}
    QML_FILES
       ConfirmDialog.qml
       ReportList.qml
       ReportListEntry.qml
       StyledButton.qml
       StyledComboBox.qml
       StyledTextField.qml
       TabIconButton.qml
       TableLoader.qml
       TableRow.qml
)
