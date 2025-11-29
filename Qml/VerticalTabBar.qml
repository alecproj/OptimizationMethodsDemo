import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AppEnums
import Globals
import Components

Flickable {
    id: root
    signal tabChanged(int tabIndex)

    contentHeight: column.implicitHeight 
    contentWidth: column.implicitWidth

    flickableDirection: Flickable.VerticalFlick

    Column {
        id: column
        padding: 10
        spacing: 5

        y: Math.max(0, (root.height - column.implicitHeight) / 2)

        TabIconButton {
            source: "qrc:/icons/home.svg"
            idx: 0
            onTabChanged: root.tabChanged(idx)
        }

        TabIconButton {
            visible: AppStates.selectedPartition !== PartType.NONE
            source: "qrc:/icons/flask-empty.svg"
            idx: 1
            onTabChanged: root.tabChanged(idx)
        }

        TabIconButton {
            visible: AppStates.selectedPartition !== PartType.NONE
            source: "qrc:/icons/folder.svg"
            idx: 2
            onTabChanged: root.tabChanged(idx)
        }

        Repeater {
            model: controller.openReportsCount

            delegate: Item {
                implicitWidth: btn.implicitWidth
                implicitHeight: btn.implicitHeight

                TabIconButton {
                    id: btn
                    source: "qrc:/icons/file.svg"
                    idx: index + 3

                    onTabChanged: root.tabChanged(idx)
                }
            }
        }
    }
}
