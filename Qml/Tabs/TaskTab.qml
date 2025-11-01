import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    color: AppPalette.background

    property int selectedAlgorithm: 1

    ColumnLayout {
        id: column
        anchors.fill: parent

        spacing: 15

        Label {
            id: title

            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: column.spacing

            text: "Постановка задачи"
            font.pixelSize: 20
            font.bold: true
        }

        StyledComboBox {
            id: algorithm
            Layout.leftMargin: (parent.width * 0.05)
            Layout.preferredWidth: 230

            model: [
                { value: 1, text: "Метод координатного спуска" },
                { value: 2, text: "Метод градиентного спуска" }
            ]

            textRole: "text"
            valueRole: "value"
            currentValue: root.selectedAlgorithm
            onActivated: root.selectedAlgorithm = currentValue
        }

        StyledTextField {
            id: formula
            Layout.preferredWidth: (parent.width * 0.8)
            Layout.alignment: Qt.AlignHCenter
            placeholderText: "Введите формулу"
        }

        Item {
            id: inputParams
            Layout.fillHeight: true
        }

        Label {
            id: reportListLabel
            Layout.leftMargin: (parent.width * 0.05) + 10
            text: "Восстановить входные данные из отчета:"
        }

        ReportList {
            id: reportList
            Layout.preferredWidth: (parent.width * 0.9)
            Layout.preferredHeight: (parent.height * 0.3)
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: column.spacing
        }
    }

}
