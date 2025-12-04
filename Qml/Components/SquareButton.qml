import QtQuick 2.15

import Globals

Item {
    id: root
    // text for the button
    property alias text: label.text
    signal clicked()

    property color defaultColor: AppPalette.background
    property color hoverColor: AppPalette.selected

    width: 100
    height: width

    Rectangle {
        id: rect
        anchors.fill: parent
        color: hovered ? root.hoverColor : root.defaultColor
        radius: 15
        border.width: 0
        smooth: true

        MouseArea {
            id: ma
            anchors.fill: parent
            hoverEnabled: true
            onClicked: root.clicked()
            onEntered: rect.hovered = true
            onExited: rect.hovered = false
        }
        property bool hovered: false

        Text {
            id: label
            anchors.centerIn: parent
            text: "Button"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            width: parent.width - 16
            font.pixelSize: Math.min(16, parent.width / 8)
        }
    }
}
