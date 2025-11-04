import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import AppEnums

Rectangle {
    id: root

    EnumHelper { id: helper }

    color: AppPalette.background

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
            Layout.preferredWidth: 230

            model: [
                { value: AlgoType.CD, text: "Метод координатного спуска" },
                { value: AlgoType.GD, text: "Метод градиентного спуска" }
            ]

            textRole: "text"
            valueRole: "value"
            currentValue: AppStates.selectedAlgorithm
            onActivated: {
                AppStates.selectedAlgorithm = currentValue;
                AppStates.selectedFullAlgo 
                    = helper.calculateFullType(
                        AppStates.selectedAlgorithm, 
                        AppStates.selectedExtension
                    );
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
                if (!/^[0-9A-Za-z\s+\-*/^().,]+$/.test(s)) return false;
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
                console.log("SetInputData: " + rv);
                rv = controller.solve();
                console.log("Solve: " + rv);
            }
        }

        Label {
            id: reportListLabel
            Layout.leftMargin: (parent.width * 0.05) + 10
            text: "Восстановить входные данные из отчета:"
        }

        ReportList {
            id: reportList
            Layout.preferredWidth: (parent.width * 0.9)
            Layout.preferredHeight: (parent.height * 0.2)
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: column.spacing
        }
    }

}
