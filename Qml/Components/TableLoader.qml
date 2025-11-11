import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


Loader {
    id: root 

    required property string title
    required property var columns
    required property var rows

    property int rowsCnt: rows.length
    property int fontSize: 14

    sourceComponent: Item {

        implicitWidth: tableView.implicitWidth
        implicitHeight: tableView.implicitHeight

        ColumnLayout {
            id: tableView
            anchors.fill: parent

            spacing: 0

            Text {
                text: root.title
                font.pixelSize: root.fontSize
                Layout.alignment: Qt.AlignHCenter
                bottomPadding: 5
            }

            TableRow {
                rowData: root.columns;
                header: true;
                fontSize: root.fontSize;
            }

            Repeater {
                id: content
                
                model: root.rows

                delegate: Item {
                    implicitHeight: row.implicitHeight
                    implicitWidth: row.implicitWidth

                    TableRow{
                        id: row
                        rowData: modelData
                        header: false
                        fontSize: root.fontSize;
                    }
                } 
            }
        }

    }
}
