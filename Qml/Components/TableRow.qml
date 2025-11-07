import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: root
    spacing: 0

    required property var rowData
    property bool header: false
    property int fontSize: 14

    Repeater {
        model: root.rowData
        
        delegate: Rectangle {
            width: 150
            implicitHeight: ceil.height

            border.color: AppPalette.foreground

            Text {
                id: ceil
                anchors.centerIn: parent
                topPadding: 4
                bottomPadding: 4
                text: modelData
                font.bold: root.header
                font.pixelSize: root.fontSize
                wrapMode: Text.WordWrap
            }
        }
    }
} // header
