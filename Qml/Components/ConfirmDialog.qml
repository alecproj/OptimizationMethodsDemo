import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property string dlgTitle: ""
    property string dlgText: ""
    property bool twoButtonsProp: false

    Connections {
        target: controller
        function onRequestConfirm(title, text, twoButtons) {
            dlgTitle = title
            dlgText = text
            twoButtonsProp = twoButtons
            confirmDialog.open()
        }
    }

    Dialog {
        id: confirmDialog
        anchors.centerIn: parent
        title: dlgTitle

        background: Rectangle {
            color: AppPalette.background
            radius: 15
            border.color: AppPalette.stroke
        }

        header: ColumnLayout {
            spacing: 8
            Text {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 10
                Layout.bottomMargin: 5
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                text: dlgTitle
                font.pixelSize: 14
                font.bold: true
                wrapMode: Text.WordWrap
            }

        }

        contentItem: Column {
            spacing: 8
            Text {
                leftPadding: 10
                rightPadding: 10
                text: dlgText
                font.pixelSize: 14
                wrapMode: Text.WordWrap
            }
        }

        footer: RowLayout {
            spacing: 8

            Item { Layout.fillWidth: true }

            StyledButton {
                Layout.preferredWidth: 100
                Layout.topMargin: 5
                Layout.bottomMargin: 8
                visible: twoButtonsProp
                text: "Отмена"
                onClicked: {
                    confirmDialog.close()
                }
            }

            StyledButton {
                Layout.preferredWidth: 100
                Layout.topMargin: 5
                Layout.bottomMargin: 8
                Layout.rightMargin: 5
                text: twoButtonsProp ? "Да" : "OK"
                bgcolor: twoButtonsProp ? AppPalette.warning : AppPalette.accent
                onClicked: {
                    confirmDialog.close()
                    if (twoButtonsProp) {
                        controller.deleteConfirmed();
                        AppStates.currentTabIndex = 1;
                    }
                }
            }
        }
    }
}

