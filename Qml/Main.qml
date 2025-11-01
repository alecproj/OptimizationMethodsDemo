import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 640
    height: 400
    visible: true
    title: "Optimization Methods Demo App"
    color: AppPalette.surface

    RowLayout {
        anchors.fill: parent
        spacing: 0

        VerticalTabBar {
            id: verticalTabs
            Layout.preferredWidth: contentWidth
            Layout.fillHeight: true
        }
    
        StackLayout {
            id: tabs
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: AppStates.currentTabIndex
    
            TaskTab {
                id: taskTab
            }
    
            Rectangle {
                color: "#ffffff"
                Label {
                    anchors.centerIn: parent
                    text: "Отчеты"
                    font.pixelSize: 20
                }
            }
    
            Rectangle {
                color: "#ffffff"
                Label {
                    anchors.centerIn: parent
                    text: "GD-31-10-25-13-48"
                    font.pixelSize: 20
                }
            }
        }
    }
}
