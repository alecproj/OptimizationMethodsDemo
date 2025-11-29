import QtQuick
import QtQuick.Layouts

import AppEnums
import Globals
import Components

Rectangle {
    id: root

    color: AppPalette.surface

    EnumHelper { id: helper }

    property int spacing: 10

    signal selected(int partType)

    function partToStr(partType) {
        return helper.partTypeToString(partType)
    }

    onWidthChanged: layout.columns = (width / 150)

    GridLayout {
        id: layout
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: root.spacing
        anchors.leftMargin: root.spacing
        anchors.rightMargin: root.spacing
        columns: 4

        property int itemSize: (root.width - (columns + 1) * root.spacing) / columns
        rowSpacing: root.spacing
        columnSpacing: root.spacing

        Layout.alignment: Qt.AlignTop

        Repeater {
            id: repeater
            property int itemsCount: 2

            model: [
                { value: PartType.LO, name: root.partToStr(PartType.LO) },
                { value: PartType.GO, name: root.partToStr(PartType.GO) }
            ]

            delegate: SquareButton {

                required property int value
                required property string name

                Layout.preferredWidth: layout.itemSize
                Layout.preferredHeight: layout.itemSize 
                text: name

                hoverColor: AppPalette.accent

                onClicked: root.selected(value)
            }
        } // repeater

        Repeater {
            model: layout.columns - repeater.itemsCount

            delegate: Item {
                Layout.preferredWidth: layout.itemSize
                Layout.preferredHeight: layout.itemSize
            }
        }
    } // layout
}

