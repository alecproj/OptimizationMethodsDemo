pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

TextField {
    id: root

    font.pixelSize: 16
    leftPadding: 15 

    background: Rectangle {
        implicitHeight: root.font.pixelSize + 20
        implicitWidth: root.width

        color: AppPalette.surface
        border.color: root.activeFocus ? AppPalette.accent : AppPalette.stroke
        radius: implicitWidth * 0.15
    }

}
