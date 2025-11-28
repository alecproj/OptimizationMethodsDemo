import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AppEnums
import Globals
import Components
import LocalOptimization as LO
import GlobalOptimization as GO

Item {
    id: root

    required property int partition
    required property string fileName
    required property var inputData
    required property var solution
    required property var resultData

    Component {
        id: lo

        LO.ReportTab {
            id: reportTab

            fileName: root.fileName
            inputData: root.inputData
            solution: root.solution
            resultData: root.resultData
        }
    }

    Loader {
        id: loader
        anchors.fill: parent
        active: (root.partition === PartType.LO)
        sourceComponent: lo
    }
}

