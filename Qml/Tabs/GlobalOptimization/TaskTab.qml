import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AppEnums
import GOInputData
import Globals
import Components

Rectangle {
    id: root

    EnumHelper { id: helper }

    color: AppPalette.background

    function algoToStr(algoType) {
        return helper.algoTypeToString(algoType)
    }

    ColumnLayout {
        id: column
        anchors.fill: parent

        spacing: 15

        Label {
            id: title

            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: column.spacing

            text: "Постановка задачи"
            font.pixelSize: 20
            font.bold: true
        }

        StyledComboBox {
            id: algorithm
            Layout.leftMargin: (parent.width * 0.05)
            Layout.preferredWidth: 250

            model: [
                { value: AlgoType.GA, text: root.algoToStr(AlgoType.GA) },
                { value: AlgoType.PS, text: root.algoToStr(AlgoType.PS) }
            ]

            textRole: "text"
            valueRole: "value"
            currentValue: AppStates.selectedAlgorithm
            onActivated: {
                AppStates.selectedAlgorithm = currentValue;
            }
        }

        StyledTextField {
            id: func
            Layout.preferredWidth: (parent.width * 0.8)
            Layout.alignment: Qt.AlignHCenter
            placeholderText: "Введите f(x,y), например exp^(1.5*x)*sin(0.002*y)"

            property bool accepted: false

            function quickFuncTest(s) {
                if (typeof s !== "string") return false;
                s = s.trim();
                if (s.length === 0) return false;
                if (!/\b(?:x|y)\b/i.test(s)) return false;
                if (!/^[0-9A-Za-z\s+\-*/^().,_]+$/.test(s)) return false;
                if (/[;`{}[\]|&<>\\]/.test(s)) return false;

                let bal = 0;
                for (let i = 0; i < s.length; ++i) {
                    const c = s[i];
                    if (c === '(') bal++;
                    else if (c === ')') {
                        bal--;
                        if (bal < 0) return false;
                    }
                }
                if (bal !== 0) return false;

                return true;
            }

            onActiveFocusChanged: {
                if (quickFuncTest(text)) {
                    func.valid = true;
                    func.accepted = true;
                } else {
                    func.valid = false;
                    func.accepted = false;
                }
            }
        }

        InputParams {
            id: inputParams
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: (parent.width * 0.9)
        }

        StyledButton {
            id: solveBtn

            Layout.alignment: Qt.AlignRight
            Layout.rightMargin: ((parent.width * 0.05) + 10)

            text: "Решить"
            enabled: inputParams.valid && func.accepted

            onReleased: {
                inputParams.inputData.function = func.text;
                var rv = controller.setInputData(inputParams.inputData.instance());
                if (rv !== 0) {
                    return;
                }
                rv = controller.solve();
                if (rv === 0) {
                    AppStates.currentTabIndex = controller.openReportsCount + 2
                    controller.updateQuickInfoModel();
                }
            }
        }

        Label {
            id: reportListLabel
            Layout.leftMargin: (parent.width * 0.05) + 10
            text: "Восстановить входные данные из отчета:"
        }

        InputData { id: sourceInputData }

        ReportList {
            id: reportList
            Layout.preferredWidth: (parent.width * 0.9)
            Layout.preferredHeight: (parent.height * 0.2)
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: column.spacing
            model: controller.quickInfoModel

            onPressed: function(fileName) {
                var rv = controller.inputDataFromFile(fileName, sourceInputData);
                if (rv === Result.Success) {
                    AppStates.selectedAlgorithm = sourceInputData.algorithmId;
                    func.text = sourceInputData.function;
                    if (func.text !== "") {
                        func.valid = true;
                        func.accepted = true;
                    } else {
                        func.valid = false;
                        func.accepted = false;
                    }
                    inputParams.setInputData(sourceInputData);
                }
            }
        }
    }

}
