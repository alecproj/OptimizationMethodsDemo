import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Globals
import Components
import LocalOptimization as LO
import GlobalOptimization as GO

ApplicationWindow {
    id: root
    width: 640
    height: 400
    visible: true
    title: "Optimization Methods Demo App"
    color: AppPalette.surface

    ConfirmDialog {
        id: dialog
        anchors.fill: parent
    }

    Component {
        id: taskTab
        LO.TaskTab {}
    }

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

            Loader {
                id: taskTabLoader 
                active: true
                sourceComponent: taskTab 
            }

    
            FolderTab {
                id: folderTab
            }

            Repeater {
                model: controller.openReportsCount

                delegate: Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    LO.ReportTab {
                        anchors.fill: parent
                        id: reportTab
                        report: controller.openReports[index]
                    }
                }
            }
        }
    }
}
