import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Flickable {
    id: root
    signal tabChanged(int tabIndex)

    contentHeight: column.implicitHeight 
    contentWidth: column.implicitWidth

    flickableDirection: Flickable.VerticalFlick

    Column {
        id: column
        padding: 10
        spacing: 5

        y: Math.max(0, (root.height - column.implicitHeight) / 2)

        TabIconButton {
            source: "qrc:/icons/flask.svg"
            idx: 0
            onTabChanged: root.tabChanged(idx)
        }

        TabIconButton {
            source: "qrc:/icons/folder.svg"
            idx: 1
            onTabChanged: root.tabChanged(idx)
        }

        Repeater {
            model: [
                { source: "qrc:/icons/file-alt.svg", idx: 2 }
            ]

            delegate: TabIconButton {

                onTabChanged: root.tabChanged(idx)
            }
        }
    }
}
