import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AppEnums
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

            HomeTab {
                id: partTab
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Loader {
                id: taskTabLoader 
                sourceComponent: {
                    if (AppStates.selectedPartition === PartType.LO) {
                        loTaskTab
                    } else if (AppStates.selectedPartition === PartType.GO) {
                        goTaskTab
                    } else {
                        null
                    }
                }
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
