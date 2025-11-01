import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    color: AppPalette.surface
    radius: 15

    property int contentPadding: 10

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
                model: [
                    { file: "GD 31.10.25 20:28", status: 0 },
                    { file: "GD 31.10.25 13:18", status: 0 },
                    { file: "GD 31.10.25 10:10", status: 0 },
                    { file: "GD 30.10.25 13:18", status: 1 }
                ]

                delegate: ReportListEntry {
                    width: (root.width - (2 * root.contentPadding))
                }
            }
        }

    }
}
