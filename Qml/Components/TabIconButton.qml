import QtQuick
import QtQuick.Controls

Button {
    id: root

    signal tabChanged()

    required property string source 
    required property int idx

    property int iconSize: 32

    icon.source: source
    icon.color: AppPalette.foreground
    icon.height: iconSize
    icon.width: iconSize

    background: Rectangle {
        implicitWidth: root.iconSize + 20
        implicitHeight: root.iconSize + 20
        
        color: {
            if (root.hovered) {
                if (AppStates.currentTabIndex === root.idx) {
                    AppPalette.withAlpha(AppPalette.accent, 0.8)
                } else {
                    AppPalette.selected
                }
            } else {
                if (AppStates.currentTabIndex === root.idx) {
                    AppPalette.accent
                } else {
                    AppPalette.surface
                }
            }
        }
        radius: width * 0.15
    }

    onClicked: {
        AppStates.currentTabIndex = idx
        tabChanged()
    }
}

