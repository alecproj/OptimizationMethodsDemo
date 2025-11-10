import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: root
    spacing: 0

    required property var rowData
    property bool header: false
    property int fontSize: 14
    property int cellWidth: 150
    property int padding: 8
    property real maxCellHeight: 0

    Item {
        id: measurer
        visible: false
        Repeater {
            model: root.rowData
            delegate: Text {
                text: modelData
                font.pixelSize: root.fontSize
                font.bold: root.header
                wrapMode: Text.Wrap
                width: root.cellWidth - root.padding*2

                Component.onCompleted: updateMaxHeight()
                onContentHeightChanged: updateMaxHeight()

                function updateMaxHeight() {
                    var h = Math.ceil(contentHeight + root.padding*2)
                    if (h > root.maxCellHeight)
                        root.maxCellHeight = h
                }
            }
        }
    }

    Repeater {
        model: root.rowData
        delegate: Rectangle {
            Layout.preferredWidth: root.cellWidth
            Layout.preferredHeight: root.maxCellHeight

            color: "transparent"
            border.color: "gray"

            Column {
                anchors.centerIn: parent
                spacing: 0

                Text {
                    text: modelData
                    font.pixelSize: root.fontSize
                    font.bold: root.header
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    width: root.cellWidth - root.padding*2
                }
            }
        }
    }
}
