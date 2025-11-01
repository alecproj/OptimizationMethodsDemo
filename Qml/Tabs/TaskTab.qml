import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    color: AppPalette.background

    property int selectedAlgorithm: 1

    Column {
        id: column
        anchors.fill: parent

        spacing: 15

        Label {
            id: title
            anchors.horizontalCenter: parent.horizontalCenter
            topPadding: column.spacing
            text: "Постановка задачи"
            font.pixelSize: 20
            font.bold: true
        }

        StyledComboBox {
            id: algorithm
            anchors.left: parent.left
            anchors.leftMargin: (parent.width * 0.05)
            width: 230

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
            width: parent.width * 0.8
            anchors.horizontalCenter: parent.horizontalCenter
            placeholderText: "Введите формулу"

        }

        Item {
            id: inputParams
            height: parent.height
                -formula.height
                -reportList.height
                -algorithm.height
                -reportListLabel.height
                -title.height
                -(6 * column.spacing)
            width: parent.width
        }

        Label {
            id: reportListLabel
            anchors.left: parent.left
            anchors.leftMargin: (parent.width * 0.05) + 10
            text: "Восстановить входные данные из отчета:"
        }

        ReportList {
            id: reportList
            width: parent.width * 0.9
            height: parent.height * 0.3
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

}
