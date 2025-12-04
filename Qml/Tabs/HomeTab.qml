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

    function partToStr(partType) {
        return helper.partTypeToString(partType)
    }

    ColumnLayout {
        id: column
        anchors.fill: parent
        spacing: 15

        Label {
            id: title

            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: column.spacing

            text: "Разделы алгоритмов оптимизации"
            font.pixelSize: 20
            font.bold: true
        }

        Item {
            Layout.preferredHeight: parent.width * 0.05
            Layout.fillWidth: true
        }

        Label {
            id: reportListLabel
            Layout.leftMargin: (parent.width * 0.05) + 10
            text: "Выберите раздел:"
            font.pixelSize: 14
        }

        PartsList {
            Layout.preferredWidth: (parent.width * 0.9)
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: column.spacing

            radius: 15

            onSelected: function(value) {
                if (value === PartType.LO) {
                    AppStates.selectedPartition = value
                    AppStates.selectedAlgorithm = AlgoType.CD
                    AppStates.selectedExtension = ExtensionType.B
                    AppStates.selectedFullAlgo = FullAlgoType.CDB
                } else if (value === PartType.GO) {
                    AppStates.selectedAlgorithm = AlgoType.GA
                    AppStates.selectedPartition = value
                }
                AppStates.currentTabIndex = 1
            }
        }

    } // column
}
