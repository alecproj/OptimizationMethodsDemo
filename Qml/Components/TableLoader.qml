import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QC

import Globals

Loader {
    id: root

    required property string title
    required property var model
    property int fontSize: 14
    property real defaultColumnWidth: 135

    // width: root.defaultColumnWidth * root.model.headers.length
    // height: (root.model.rowCount() + 1) * 50
    sourceComponent: Item {
        implicitWidth: (root.defaultColumnWidth * root.model.headers.length) + 4.5
        // implicitHeight: (root.model.rowCount() + 1) * 50

        QC.HorizontalHeaderView {
            id: headers
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            syncView: table
            clip: false

            delegate: Rectangle {
                width: table.columnWidthProvider(table.column)
                implicitHeight: 50

                border.color: AppPalette.foreground

                 Text {
                    text: display
                    wrapMode: Text.Wrap
                    anchors.fill: parent
                    font.bold: true
                    leftPadding: 3
                    rightPadding: 3
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        TableView {
            id: table
            anchors.top: headers.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            columnSpacing: 0
            rowSpacing: 0
            clip: true
            interactive: false

            columnWidthProvider: function (column) {
                return root.defaultColumnWidth;
            }

            model: root.model

            delegate: Rectangle {
                width: table.columnWidthProvider(table.column)
                implicitHeight: 50

                border.color: AppPalette.foreground

                 Text {
                    text: display
                    wrapMode: Text.Wrap
                    anchors.fill: parent
                    leftPadding: 3
                    rightPadding: 3
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            QC.ScrollBar.vertical: QC.ScrollBar{ 
                id: vScroll
                policy: (table.contentHeight > table.height) ? QC.ScrollBar.AlwaysOn : QC.ScrollBar.AlwaysOff
            }
        } // TableView
    } // sourceComponent
}
