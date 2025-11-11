import QtQuick
import QtQuick.Controls

Button {
    id: root

    height: txt.font.pixelSize + 16

    required property string name
    required property string info
    required property int status

    icon.source: {
        if (root.status === 0) {
            "qrc:/icons/file-alt.svg" 
        } else {
            "qrc:/icons/file-xmark.svg"
        }
    }

    contentItem: Row {
        id: content
        spacing: 10
        anchors.left: parent.left
        anchors.leftMargin: spacing
        anchors.right: parent.right
        anchors.rightMargin: spacing
        anchors.verticalCenter: parent.verticalCenter

        Image {
            source: root.icon.source
            width: txt.font.pixelSize
            height: width
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            id: txt

            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            text: root.info
            font.pixelSize: 16
            color: AppPalette.foreground
        }
    }

    background: Rectangle {
        color: root.hovered ? AppPalette.selected : AppPalette.surface
        radius: width * 0.15
    }

}
