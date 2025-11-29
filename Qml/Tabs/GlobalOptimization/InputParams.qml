import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AppEnums
import GOInputData
import Globals
import Components

Flickable {
    id: root

    contentWidth: column.implicitWidth
    contentHeight: column.implicitHeight
    flickableDirection: Flickable.VerticalFlick
    clip: true

    property alias inputData: inputData

    InputData {
        id: inputData
        algorithmId: AppStates.selectedAlgorithm
    }

    onInputDataChanged: console.log(inputData)

    EnumHelper { id: helper }

    function extremumToStr(extremumType) {
        return helper.extremumTypeToString(extremumType)
    }

    property int checkMask: helper.getCheckByAlgoType(AppStates.selectedAlgorithm)

    property int flags: 0
    property bool valid: false

    function setInputData(data) {
        inputData.algorithmId = AppStates.selectedAlgorithm;
        extremum.selected = data.extremumId;
        inputData.extremumId = data.extremumId;
        calcAccuracy.text = data.calcAccuracyAsString();
        inputData.calcAccuracy = data.calcAccuracy;
        resultAccuracy.text = data.resultAccuracyAsString();
        inputData.resultAccuracy = data.resultAccuracy;
        minX.text = data.minXAsString();
        inputData.minX = data.minX;
        maxX.text = data.maxXAsString();
        inputData.maxX = data.maxX;
        minY.text = data.minYAsString();
        inputData.minY = data.minY;
        maxY.text = data.maxYAsString();
        inputData.maxY = data.maxY;
        flags = helper.getCheckByAlgoType(AppStates.selectedAlgorithm);
        valid = true;
    }

    function setFlag(flag) {
        flags = flags | flag
    }

    function clearFlag(flag) {
        flags = flags & (~flag)
    }

    function validate() {
        var mask = helper.getCheckByAlgoType(AppStates.selectedAlgorithm);
        if ((flags & mask) ===  mask) {
            return true;
        }
        return false;
    }

    ColumnLayout {
        id: column
        width: Math.max(root.width - vScroll.implicitWidth - 2, implicitWidth)
        spacing: 5

        RowLayout {
            spacing: 10

            Text {
                text: "Укажите тип экстремума"
            }
        
            Item { Layout.fillWidth: true }
        
            StyledComboBox {
                id: extremum
                Layout.preferredWidth: 100
        
                property int selected: ExtremumType.MINIMUM
        
                model: [
                    { value: ExtremumType.MINIMUM, text: root.extremumToStr(ExtremumType.MINIMUM) },
                    { value: ExtremumType.MAXIMUM, text: root.extremumToStr(ExtremumType.MAXIMUM) }
                ]
        
                textRole: "text"
                valueRole: "value"
                currentValue: extremum.selected
                onActivated: {
                    extremum.selected = currentValue;
                    inputData.extremumId = currentValue;
                }
            }
        }

        // CalcAccuracy
        RowLayout {
            visible: (root.checkMask & CheckList.CalcAccuracy)
            spacing: 10

            Text {
                text: "Укажите точность вычислений (кол-во знаков после запятой)"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: calcAccuracy 
                boxed: true
                Layout.preferredWidth: 150
                placeholderText: "8"

                validator: RegularExpressionValidator {
                    regularExpression: /^(?:[1-9]|1[0-5])$/i
                }

                onTextEdited: {
                    if (acceptableInput 
                        && inputData.setCalcAccuracyFromString(text)) {
                        calcAccuracy.valid = true;
                        root.setFlag(CheckList.CalcAccuracy);
                    } else {
                        calcAccuracy.valid = false;
                        root.clearFlag(CheckList.CalcAccuracy);
                    }
                    root.valid = root.validate();
                }
            }
        }
        
        // ResultAccuracy
        RowLayout {
            visible: (root.checkMask & CheckList.ResultAccuracy)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите точность результата (кол-во знаков после запятой)"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: resultAccuracy
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "6"

                validator: RegularExpressionValidator {
                    regularExpression: /^(?:[1-9]|1[0-5])$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setResultAccuracyFromString(text)) {
                        resultAccuracy.valid = true
                        root.setFlag(CheckList.ResultAccuracy)
                    } else {
                        resultAccuracy.valid = false
                        root.clearFlag(CheckList.ResultAccuracy)
                    }
                    root.valid = root.validate()
                }
            }
        }

        // MinX
        // MaxX
        RowLayout {
            visible: (root.checkMask & CheckList.MinX) && (root.checkMask & CheckList.MaxX)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите диапазон по оси X"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: minX
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "-10.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^\-?\d{1,15}\.\d{0,15}([eE][+-]?\d{1,3})?$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setMinXFromString(text)) {
                        minX.valid = true
                        root.setFlag(CheckList.MinX)
                    } else {
                        minX.valid = false
                        root.clearFlag(CheckList.MinX)
                    }
                    root.valid = root.validate()
                }
            }

            StyledTextField {
                id: maxX
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "10.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^\-?\d{1,15}\.\d{0,15}([eE][+-]?\d{1,3})?$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setMaxXFromString(text)) {
                        maxX.valid = true
                        root.setFlag(CheckList.MaxX)
                    } else {
                        maxX.valid = false
                        root.clearFlag(CheckList.MaxX)
                    }
                    root.valid = root.validate()
                }
            }
        } 


        // MinY
        // MaxY
        RowLayout {
            visible: (root.checkMask & CheckList.MaxY) && (root.checkMask & CheckList.MinY)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите диапазон по оси Y"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: minY
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "-10.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^\-?\d{1,15}\.\d{0,15}([eE][+-]?\d{1,3})?$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setMinYFromString(text)) {
                        minY.valid = true
                        root.setFlag(CheckList.MinY)
                    } else {
                        minY.valid = false
                        root.clearFlag(CheckList.MinY)
                    }
                    root.valid = root.validate()
                }
            }

            StyledTextField {
                id: maxY
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "10.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^\-?\d{1,15}\.\d{0,15}([eE][+-]?\d{1,3})?$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setMaxYFromString(text)) {
                        maxY.valid = true
                        root.setFlag(CheckList.MaxY)
                    } else {
                        maxY.valid = false
                        root.clearFlag(CheckList.MaxY)
                    }
                    root.valid = root.validate()
                }
            }
        } 

    } // Column

    ScrollBar.vertical: ScrollBar{ 
        id: vScroll
        policy: (root.contentHeight > root.height) ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
    }
    
    ScrollBar.horizontal: ScrollBar{ 
        id: hScroll
        policy: (root.contentWidth > root.width) ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
    }
}
