import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AppEnums
import LOInputData
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
        extensionId: AppStates.selectedExtension
        fullAlgoId: AppStates.selectedFullAlgo
    }

    onInputDataChanged: console.log(inputData)

    EnumHelper { id: helper }

    property int checkMask: helper.getCheckByFullType(AppStates.selectedFullAlgo)

    property int flags: 0
    property bool valid: false

    function setInputData(data) {
        inputData.algorithmId = AppStates.selectedAlgorithm;
        inputData.extensionId = AppStates.selectedExtension;
        inputData.fullAlgoId = AppStates.selectedFullAlgo;
        extremum.selected = data.extremumId;
        inputData.extremumId = data.extremumId;
        stepType.selected = data.stepId;
        inputData.stepId = data.stepId;
        iterations.text = data.maxIterationsAsString();
        inputData.maxIterations = data.maxIterations;
        funcCalls.text = data.maxFuncCallsAsString();
        inputData.maxFuncCalls = data.maxFuncCalls;
        calcAccuracy.text = data.calcAccuracyAsString();
        inputData.calcAccuracy = data.calcAccuracy;
        resultAccuracy.text = data.resultAccuracyAsString();
        inputData.resultAccuracy = data.resultAccuracy;
        startX1.text = data.startX1AsString();
        inputData.startX1 = data.startX1;
        startY1.text = data.startY1AsString();
        inputData.startY1 = data.startY1;
        startX2.text = data.startX2AsString();
        inputData.startX2 = data.startX2;
        startY2.text = data.startY2AsString();
        inputData.startY2 = data.startY2;
        stepX.text = data.stepXAsString();
        inputData.stepX = data.stepX;
        stepY.text = data.stepYAsString();
        inputData.stepY = data.stepY;
        step.text = data.stepAsString();
        inputData.step = data.step;
        minX.text = data.minXAsString();
        inputData.minX = data.minX;
        maxX.text = data.maxXAsString();
        inputData.maxX = data.maxX;
        minY.text = data.minYAsString();
        inputData.minY = data.minY;
        maxY.text = data.maxYAsString();
        inputData.maxY = data.maxY;
        flags = helper.getCheckByFullType(AppStates.selectedFullAlgo);
        valid = true;
    }

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
                Layout.preferredWidth: 150
        
                property int selected: ExtremumType.MINIMUM
        
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
            visible: ((AppStates.selectedFullAlgo !== FullAlgoType.GDS)
                && (AppStates.selectedFullAlgo !== FullAlgoType.GDR)
                && (AppStates.selectedFullAlgo !== FullAlgoType.CGB))
            spacing: 10

            Text {
                text: "Укажите тип шага"
            }
        
            Item { Layout.fillWidth: true }
        
            StyledComboBox {
                id: stepType
                Layout.preferredWidth: 160
        
                property int selected: StepType.CONSTANT
        
                model: [
                    { value: StepType.CONSTANT, text: "Константный" },
                    { value: StepType.COEFFICIENT, text: "Коэффициентный" },
                    { value: StepType.ADAPTIVE, text: "Адаптивный" }
                ]
        
                textRole: "text"
                valueRole: "value"
                currentValue: stepType.selected
                onActivated: {
                    stepType.selected = currentValue;
                    inputData.stepId = currentValue;
                }
            }
        }

        RowLayout {
            visible: (AppStates.selectedFullAlgo !== FullAlgoType.CGB)
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
        
        // ResultAccuracy = (1 << 1)
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

        // StartX1 = (1 << 2)
        // StartY1 = (1 << 3)
        RowLayout {
            visible: (root.checkMask & CheckList.StartY1) && (root.checkMask & CheckList.StartY1)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите начальное приближение по координатам x, y"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: startX1
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "1.0"

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
                placeholderText: "1.0"

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
            visible: (root.checkMask & CheckList.StartY2) && (root.checkMask & CheckList.startX2)
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
            visible: (root.checkMask & CheckList.StepY) && (root.checkMask & CheckList.StepX)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: stepType.selected == StepType.COEFFICIENT
                ? "Укажите значение коэффициентного шага по координатам x, y"
                : "Укажите значение константного шага по координатам x, y"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: stepX
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "0.1"

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
                placeholderText: "0.1"

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

        // Step = (1 << 8)
        RowLayout {
            visible: (root.checkMask & CheckList.Step)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: (stepType.selected == StepType.COEFFICIENT)
                ? "Укажите значение коэффициентного шага"
                : "Укажите значение константного шага"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: step
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "0.1"

                validator: RegularExpressionValidator {
                    regularExpression: /^\-?\d{1,5}\.\d{0,15}([eE][+-]?\d{1,3})?$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setStepFromString(text)) {
                        step.valid = true
                        root.setFlag(CheckList.Step)
                    } else {
                        step.valid = false
                        root.clearFlag(CheckList.Step)
                    }
                    root.valid = root.validate()
                }
            }
        }

        // MinX = (1 << 9)
        // MaxX = (1 << 10)
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


        // MinY = (1 << 11)
        // MaxY = (1 << 12)
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

        // Iterations = (1 << 13)
        RowLayout {
            visible: (root.checkMask & CheckList.Iterations)
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
                placeholderText: "250"

                validator: RegularExpressionValidator {
                    regularExpression: /^[1-9]\d{0,3}$/i
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

        // FuncCalls = (1 << 14)
        RowLayout {
            visible: (root.checkMask & CheckList.FuncCalls)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите максимальное количество вызовов исследуемой функции"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: funcCalls 
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "2500"

                validator: RegularExpressionValidator {
                    regularExpression: /^[1-9]\d{0,4}$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setMaxFuncCallsFromString(text)) {
                        funcCalls.valid = true
                        root.setFlag(CheckList.FuncCalls)
                    } else {
                        funcCalls.valid = false
                        root.clearFlag(CheckList.FuncCalls)
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
