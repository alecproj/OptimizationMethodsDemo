import QtQuick

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

    Component {
        id: go

        GO.ReportTab {
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
        sourceComponent: {
            if (root.partition === PartType.LO) {
                lo
            } else if (root.partition === PartType.GO) {
                go
            } else {
                null
            }
        }
    }
}

