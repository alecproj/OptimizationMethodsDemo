import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import AppEnums
import InputData

Flickable {
    id: root

    contentWidth: column.implicitWidth
    contentHeight: column.implicitHeight
    flickableDirection: Flickable.HorizontalAndVerticalFlick
    clip: true

    property alias inputData: inputData

    InputData {
        id: inputData
        algorithmId: AppStates.selectedAlgorithm
        extensionId: AppStates.selectedExtension
        fullAlgoId: AppStates.selectedFullAlgo
    }

    EnumHelper { id: helper }

    property int flags: 0
    property bool valid: false

    function setFlag(flag) {
        flags = flags | flag
    }

    function clearFlag(flag) {
        flags = flags & (~flag)
    }

    function validate() {
        var mask = helper.getCheckByFullType(AppStates.selectedFullAlgo);
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
        
                property int selected: 0
        
                model: [
                    { value: ExtremumType.MINIMUM, text: "Минимум" },
                    { value: ExtremumType.MAXIMUM, text: "Максимум" },
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

        RowLayout {
            spacing: 10
        
            Text {
                text: "Укажите тип расширения"
            }
        
            Item { Layout.fillWidth: true }
        
            StyledComboBox {
                id: extension 
                Layout.preferredWidth: 230
        
                model: {
                    if (AppStates.selectedAlgorithm === 1) {
                        [
                            { value: ExtensionType.B, text: "Базовый" },
                            { value: ExtensionType.S, text: "Метод наискорейшего спуска" },
                        ]
                    } else if (AppStates.selectedAlgorithm === 2) {
                        [
                            { value: ExtensionType.B, text: "Базовый" },
                            { value: ExtensionType.S, text: "Метод наискорейшего спуска" },
                            { value: ExtensionType.R, text: "Овражный метод" }
                        ]
                    }
                }
        
                textRole: "text"
                valueRole: "value"
                currentValue: AppStates.selectedExtension
                onActivated: {
                    AppStates.selectedExtension = currentValue;
                    AppStates.selectedFullAlgo 
                        = helper.calculateFullType(
                            AppStates.selectedAlgorithm, 
                            AppStates.selectedExtension
                        );
                }
            }
        }

        // CalcAccuracy = (1 << 0)
        RowLayout {
            spacing: 10

            Text {
                text: "Укажите точность вычислений "
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: calcAccuracy 
                boxed: true
                Layout.preferredWidth: 150
                placeholderText: "0.00001"

                validator: RegularExpressionValidator {
                    regularExpression: /^(1\.0|0\.0{0,17}1)$/i
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
        
        // ResultAccuracy = (1 << 1)
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите точность результата"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: resultAccuracy
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "0.0001"

                validator: RegularExpressionValidator {
                    regularExpression: /^(1\.0|0\.0{0,17}1)$/i
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

        // StartX1 = (1 << 2)
        // StartY1 = (1 << 3)
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: (AppStates.selectedExtension === ExtensionType.R)
                ? "Укажите первую стартовую координату (x1, y1)"
                : "Укажите начальное приближение по координатам x, y"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: startX1
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "10.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^\-?\d{1,15}\.\d{0,15}([eE][+-]?\d{1,3})?$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput 
                        && inputData.setStartX1FromString(text)) {
                        startX1.valid = true
                        root.setFlag(CheckList.StartX1)
                    } else {
                        startX1.valid = false
                        root.clearFlag(CheckList.StartX1)
                    }
                    root.valid = root.validate()
                }
            }

            StyledTextField {
                id: startY1
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "10.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^\-?\d{1,15}\.\d{0,15}([eE][+-]?\d{1,3})?$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setStartY1FromString(text)) {
                        startY1.valid = true
                        root.setFlag(CheckList.StartY1)
                    } else {
                        startY1.valid = false
                        root.clearFlag(CheckList.StartY1)
                    }
                    root.valid = root.validate()
                }
            }
        }

        // StartX2 = (1 << 4)
        // StartY2 = (1 << 5)
        RowLayout {
            visible: (AppStates.selectedFullAlgo === FullAlgoType.GDR) 
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите вторую стартовую координату (x2, y2)"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: startX2
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "10.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^\-?\d{1,15}\.\d{0,15}([eE][+-]?\d{1,3})?$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setStartX2FromString(text)) {
                        startX2.valid = true
                        root.setFlag(CheckList.StartX2)
                    } else {
                        startX2.valid = false
                        root.clearFlag(CheckList.StartX2)
                    }
                    root.valid = root.validate()
                }
            }

            StyledTextField {
                id: startY2
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "10.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^\-?\d{1,15}\.\d{0,15}([eE][+-]?\d{1,3})?$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setStartY2FromString(text)) {
                        startY2.valid = true
                        root.setFlag(CheckList.StartY2)
                    } else {
                        startY2.valid = false
                        root.clearFlag(CheckList.StartY2)
                    }
                    root.valid = root.validate()
                }
            }
        }

        // StepX = (1 << 6)
        // StepY = (1 << 7)
        RowLayout {
            visible: (AppStates.selectedFullAlgo === FullAlgoType.CDB)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите значение шага по координатам x, y"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: stepX
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "10.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^\-?\d{1,15}\.\d{0,15}([eE][+-]?\d{1,3})?$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setStepXFromString(text)) {
                        stepX.valid = true
                        root.setFlag(CheckList.StepX)
                    } else {
                        stepX.valid = false
                        root.clearFlag(CheckList.StepX)
                    }
                    root.valid = root.validate()
                }
            }

            StyledTextField {
                id: stepY
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "10.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^\-?\d{1,15}\.\d{0,15}([eE][+-]?\d{1,3})?$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setStepYFromString(text)) {
                        stepY.valid = true
                        root.setFlag(CheckList.StepY)
                    } else {
                        stepY.valid = false
                        root.clearFlag(CheckList.StepY)
                    }
                    root.valid = root.validate()
                }
            }
        } 

        // CoefficientStep = (1 << 8)
        RowLayout {
            visible: (AppStates.selectedFullAlgo === FullAlgoType.GDB) ||
                     (AppStates.selectedFullAlgo === FullAlgoType.GDR)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите значение коэффициентного шага"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: coefficientStep
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "1.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^\-?\d{1,5}\.\d{0,15}([eE][+-]?\d{1,3})?$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setCoefficientStepFromString(text)) {
                        coefficientStep.valid = true
                        root.setFlag(CheckList.CoefficientStep)
                    } else {
                        coefficientStep.valid = false
                        root.clearFlag(CheckList.CoefficientStep)
                    }
                    root.valid = root.validate()
                }
            }
        }

        // MinX = (1 << 9)
        // MaxX = (1 << 10)
        RowLayout {
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
                placeholderText: "-1.0"

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
                placeholderText: "1.0"

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


        // MinY = (1 << 11)
        // MaxY = (1 << 12)
        RowLayout {
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
                placeholderText: "-1.0"

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
                placeholderText: "1.0"

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

        // Iterations = (1 << 13)
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите максимальное количество итераций"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: iterations 
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "200"

                validator: RegularExpressionValidator {
                    regularExpression: /^[1-9]\d{0,2}$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setMaxIterationsFromString(text)) {
                        iterations.valid = true
                        root.setFlag(CheckList.Iterations)
                    } else {
                        iterations.valid = false
                        root.clearFlag(CheckList.Iterations)
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
