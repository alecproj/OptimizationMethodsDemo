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
        id: loTaskTab
        LO.TaskTab {}
    }

    Component {
        id: goTaskTab
        GO.TaskTab {}
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
                sourceComponent: loTaskTab 
            }

            FolderTab {
                id: folderTab
            }

            Repeater {
                model: controller.openReports

                delegate: ReportTab {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
