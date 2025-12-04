import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Components

Item {
    id: root

    required property var entry
    property int fontSize: 14

    implicitWidth: tableLoader.active ? (tableLoader.implicitWidth + 20) : implicitWidth
    implicitHeight: (valueLoader.active
        ? valueLoader.implicitHeight : messageLoader.active
            ? messageLoader.implicitHeight : (tableLoader.height + titleLoader.height))

    Loader {
        id: valueLoader
        active: (root.entry.type === "value")

        sourceComponent: Text {
            text: root.entry.text + ": " + root.entry.value
            font.pixelSize: root.fontSize
            wrapMode: Text.WordWrap
        }
    }

    Loader {
        id: messageLoader
        active: (root.entry.type === "message")

        sourceComponent: Text {
            width: flickable.width
            text: root.entry.text
            font.pixelSize: root.fontSize
            wrapMode: Text.WordWrap
        }
    }

    Loader {
        id: titleLoader
        active: (root.entry.type === "table")
        anchors.top: parent.top
        anchors.right: tableLoader.right
        anchors.left: tableLoader.left

        sourceComponent: Text {
            width: tableLoader.width
            text: "Таблица: " + root.entry.text
            font.italic: true
            font.pixelSize: root.fontSize
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            bottomPadding: 5
        }
    }

    TableLoader {
        id: tableLoader
        active: (root.entry.type === "table")

        anchors.top: titleLoader.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        height: 500

        title: root.entry.text
        model: root.entry.table

        fontSize: root.fontSize
        onLoaded: height = Math.min(500, (model.rowCount()+1) * 50)
    }
}

