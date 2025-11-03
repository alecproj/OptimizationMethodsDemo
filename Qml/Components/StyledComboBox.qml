pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic

ComboBox {
    id: root

    property int vSpacing: 10
    topPadding: 13
    bottomPadding: 13
    spacing: 0

    delegate: ItemDelegate {
        id: delegate

        required property var model
        required property int index

        background: Rectangle {
            anchors.fill: parent
            color: delegate.hovered ? AppPalette.selected : "transparent"
            radius: width * 0.15
        }

        width: root.width
        contentItem: Text {
            text: delegate.model[root.textRole]
            color: AppPalette.foreground
            font: root.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        highlighted: root.highlightedIndex === index
    }

    indicator: Canvas {
        id: canvas
        x: root.width - width - root.vSpacing
        y: root.topPadding + (root.availableHeight - height) / 2
        width: 12
        height: 8
        contextType: "2d"

        Connections {
            target: root
            function onPressedChanged() { canvas.requestPaint(); }
        }

        onPaint: {
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = AppPalette.foreground;
            context.fill();
        }
    }

    contentItem: Text {
        leftPadding: root.vSpacing
        rightPadding: (root.vSpacing * 1.5)

        text: root.displayText
        font: root.font
        color: AppPalette.foreground
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: root.contentItem.implicitWidth + root.indicator.width + 3 * root.vSpacing
        implicitHeight: root.contentItem.implicitHeight + root.topPadding + root.bottomPadding
        // implicitHeight: 40
        color: AppPalette.surface
        radius: width * 0.15
        border.color: root.activeFocus ? AppPalette.accent : "transparent"
    }

    popup: Popup {
        y: root.height
        width: root.width
        height: Math.min(contentItem.implicitHeight, root.Window.height - topMargin - bottomMargin)
        padding: 0

        contentItem: ListView {
            id: popupContent
            clip: true
            implicitHeight: contentHeight
            model: root.popup.visible ? root.delegateModel : null
            currentIndex: root.highlightedIndex
            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            color: AppPalette.background
        }
    }
}
