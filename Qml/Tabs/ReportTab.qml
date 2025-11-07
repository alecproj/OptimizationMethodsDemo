pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: AppPalette.background

    required property var report
    property int fontSize: 14

    Flickable {
        id: flickable
        width: (parent.width * 0.9)
        height: parent.height
        anchors.centerIn: parent
        contentWidth: column.implicitWidth
        contentHeight: column.implicitHeight
        flickableDirection: Flickable.HorizontalAndVerticalFlick

        clip: true

        ColumnLayout {
            id: column
            anchors.fill: parent

            spacing: 5

            Text {
                text: "Решение"
                topPadding: 10
                font.pixelSize: 16
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            Repeater {
                model: root.report.solution

                delegate: Item {
                    id: container

                    required property string type
                    required property string title
                    required property string text
                    required property double value
                    required property var columns
                    required property var rows

                    implicitWidth: (valueLoader.active 
                        ? valueLoader.implicitWidth : messageLoader.active
                            ? messageLoader.implicitWidth : flickable.width)
                    implicitHeight: (valueLoader.active
                        ? valueLoader.implicitHeight : messageLoader.active
                            ? messageLoader.implicitHeight : tableLoader.implicitHeight)

                    Loader {
                        id: valueLoader
                        active: (type === "value")

                        sourceComponent: Text {
                            text: container.title + ": " + container.value
                            font.pixelSize: root.fontSize
                            wrapMode: Text.WordWrap
                        }
                    }

                    Loader {
                        id: messageLoader
                        active: (type === "message")

                        sourceComponent: Text {
                            width: flickable.width
                            text: container.text
                            font.pixelSize: root.fontSize
                            wrapMode: Text.WordWrap
                        }
                    }

                    TableLoader {
                        id: tableLoader
                        active: (type === "table")

                        anchors.centerIn: parent

                        title: container.title
                        columns: container.columns
                        rows: container.rows

                        fontSize: root.fontSize
                    }
                }

            }
        } // column
    } // Flickable
}
