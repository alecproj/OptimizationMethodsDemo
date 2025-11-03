import QtQuick
import QtQuick.Controls

TextField {
    id: root

    font.pixelSize: 16
    leftPadding: 15 
    rightPadding: 15

    property bool boxed: false
    property bool valid: true

    background: Rectangle {
        id: bg
        implicitHeight: root.font.pixelSize + 20

        color: AppPalette.surface
        border.color: {
            if (root.activeFocus) {
                AppPalette.accent
            } else if (!root.valid) {
                "red"
            } else {
                AppPalette.stroke
            }
        }
        radius: root.boxed ? (height * 0.3) : (width * 0.15)
    }

    Keys.onReturnPressed: {
        root.focus = false
    }
}
