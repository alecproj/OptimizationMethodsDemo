import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AppEnums
import Globals
import Components

Rectangle {
    id: root

    EnumHelper { id: helper }

    color: AppPalette.background

    ColumnLayout {
        id: column
        anchors.fill: parent

        spacing: 15

        Label {
            id: title

            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: column.spacing

            text: "Папка с отчетами"
            font.pixelSize: 20
            font.bold: true
        }

        Item { Layout.fillHeight: true }

        Label {
            id: reportListLabel
            Layout.leftMargin: (parent.width * 0.05) + 10
            text: "Открыть отчет:"
        }

        ReportList {
            id: reportList
            Layout.preferredWidth: (parent.width * 0.9)
            Layout.preferredHeight: (parent.height * 0.8)
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: column.spacing
            model: controller.quickInfoModel

            onPressed: function(fileName) {
                if (controller.openReport(fileName) == 0) {
                    AppStates.currentTabIndex = controller.openReportsCount + 1;
                }
            }
        }
    }

}
