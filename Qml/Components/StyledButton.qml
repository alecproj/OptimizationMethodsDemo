import QtQuick
import QtQuick.Controls

import Globals

Button {
    id: root

    font.pixelSize: 16
    property int margins: 5
    property string bgcolor: AppPalette.accent

    contentItem: Text {
        id: content
        text: root.text
        font: root.font
        color: AppPalette.foreground
        opacity: enabled ? 1.0 : 0.3
        leftPadding: root.margins
        rightPadding: root.margins
        topPadding: root.margins
        bottomPadding: root.margins
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitHeight: content.implicitHeight
        implicitWidth: content.implicitWidth
        color: {
            if (root.enabled) {
                if (root.hovered) {
                    AppPalette.withAlpha(root.bgcolor, 0.8)
                } else {
                    root.bgcolor 
                }
            } else {
                AppPalette.withAlpha(root.bgcolor, 0.4)
            }
        }
        radius: width * 0.15
    }
}
