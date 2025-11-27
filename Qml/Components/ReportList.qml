import QtQuick
import QtQuick.Controls

import Globals

Rectangle {
    id: root

    color: AppPalette.surface
    radius: 15

    required property var model
    property int contentPadding: 10

    signal pressed(string fileName)

    Flickable {
        id: list

        width: root.width
        height: root.height

        contentWidth: root.width
        contentHeight: column.implicitHeight

        clip: true

        Column {
            id: column
            padding: root.contentPadding

            Repeater {
                model: root.model
                delegate: ReportListEntry {
                    width: (root.width - (2 * root.contentPadding))
                    onReleased: root.pressed(name)
                }
            }
        }

    }
}
