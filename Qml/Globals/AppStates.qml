pragma Singleton

import QtQuick
import AppEnums

QtObject {
    property int currentTabIndex: 0 
    property int selectedAlgorithm: AlgoType.CD
    property int selectedExtension: ExtensionType.B
    property int selectedFullAlgo: FullAlgoType.CDB
    property int selectedPartition: PartType.NONE
}
