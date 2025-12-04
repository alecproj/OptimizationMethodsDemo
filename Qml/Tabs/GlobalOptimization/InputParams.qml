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
        size.text = data.sizeAsString();
        inputData.size = data.size;
        maxIterations.text = data.maxIterationsAsString();
        inputData.maxIterations = data.maxIterations;
        crossoverProb.text = data.crossoverProbAsString();
        inputData.crossoverProb = data.crossoverProb;
        mutationProb.text = data.mutationProbAsString();
        inputData.mutationProb = data.mutationProb;
        elitism.text = data.elitismAsString();
        inputData.elitism = data.elitism;
        inertiaCoef.text = data.inertiaCoefAsString();
        inputData.inertiaCoef = data.inertiaCoef;
        cognitiveCoef.text = data.cognitiveCoefAsString();
        inputData.cognitiveCoef = data.cognitiveCoef;
        socialCoef.text = data.socialCoefAsString();
        inputData.socialCoef = data.socialCoef;
        
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

        //------------- Common for GO -------------
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
                text: {
                    if (inputData.algorithmId === AlgoType.GA) {
                        "Укажите длину кода одной координаты особи (2-52)"
                    } else if (inputData.algorithmId === AlgoType.PS) {
                        "Укажите точность вычислений (кол-во знаков после запятой)"
                    } else {
                        "Ошибка"
                    }
                }
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: calcAccuracy 
                boxed: true
                Layout.preferredWidth: 150
                placeholderText: {
                    if (inputData.algorithmId === AlgoType.GA) {
                        "32"
                    } else if (inputData.algorithmId === AlgoType.PS) {
                        "8"
                    } else {
                        "Ошибка"
                    }
                }

                validator: RegularExpressionValidator {
                    regularExpression: {
                        if (inputData.algorithmId === AlgoType.GA) {
                            /^(?:[2-9]|[1-4]\d|5[0-2])$/i
                        } else if (inputData.algorithmId === AlgoType.PS) {
                            /^(?:[1-9]|1[0-5])$/i
                        }
                    }
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

        // Size
        RowLayout {
            visible: (root.checkMask & CheckList.Size)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: {
                    if (inputData.algorithmId === AlgoType.GA) {
                        "Укажите размер популяции (кол-во особей)"
                    } else if (inputData.algorithmId === AlgoType.PS){
                        "Укажите размер роя (кол-во частиц)"
                    } else {
                        "Ошибка"
                    }
                }
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: size 
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "50"

                validator: RegularExpressionValidator {
                    regularExpression: /^[1-9]\d{0,2}$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setSizeFromString(text)) {
                        size.valid = true
                        root.setFlag(CheckList.Size)
                    } else {
                        size.valid = false
                        root.clearFlag(CheckList.Size)
                    }
                    root.valid = root.validate()
                }
            }
        }

        // MaxIterations
        RowLayout {
            visible: (root.checkMask & CheckList.MaxIterations)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: {
                    if (inputData.algorithmId === AlgoType.GA) {
                        "Укажите количество поколений"
                    } else if (inputData.algorithmId === AlgoType.PS){
                        "Укажите максимальное количество итераций"
                    } else {
                        "Ошибка"
                    }
                }
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: maxIterations 
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "1000"

                validator: RegularExpressionValidator {
                    regularExpression: /^[1-9]\d{0,3}$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setMaxIterationsFromString(text)) {
                        maxIterations.valid = true
                        root.setFlag(CheckList.MaxIterations)
                    } else {
                        maxIterations.valid = false
                        root.clearFlag(CheckList.MaxIterations)
                    }
                    root.valid = root.validate()
                }
            }
        }

        //------------ Genetic Algorithm specific -----------------

        // Elitism
        RowLayout {
            visible: (root.checkMask & CheckList.Elitism)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите количество элитных особей"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: elitism
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "5"

                validator: RegularExpressionValidator {
                    regularExpression: /^[1-9]\d{0,1}$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setElitismFromString(text)) {
                        elitism.valid = true
                        root.setFlag(CheckList.Elitism)
                    } else {
                        elitism.valid = false
                        root.clearFlag(CheckList.Elitism)
                    }
                    root.valid = root.validate()
                }
            }
        }

        // CrossoverProb
        RowLayout {
            visible: (root.checkMask & CheckList.CrossoverProb)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите вероятность скрещивания"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: crossoverProb 
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "0.7"

                validator: RegularExpressionValidator {
                    regularExpression: /^(0?\.\d{1,15}|[0-1]\.0)$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setCrossoverProbFromString(text)) {
                        crossoverProb.valid = true
                        root.setFlag(CheckList.CrossoverProb)
                    } else {
                        crossoverProb.valid = false
                        root.clearFlag(CheckList.CrossoverProb)
                    }
                    root.valid = root.validate()
                }
            }
        }

        // MutationProb
        RowLayout {
            visible: (root.checkMask & CheckList.MutationProb)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите вероятность мутации (не более 0.1)"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: mutationProb 
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "0.05"

                validator: RegularExpressionValidator {
                    regularExpression: /^(0\.0\d{0,15}|0\.1)$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setMutationProbFromString(text)) {
                        mutationProb.valid = true
                        root.setFlag(CheckList.MutationProb)
                    } else {
                        mutationProb.valid = false
                        root.clearFlag(CheckList.MutationProb)
                    }
                    root.valid = root.validate()
                }
            }
        }

        // ----------- Particle Swarm specific -------------

        // InertiaCoef 
        RowLayout {
            visible: (root.checkMask & CheckList.InertiaCoef)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите значение коэффициента инерции (0.0-1.5)"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: inertiaCoef 
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "0.7"

                validator: RegularExpressionValidator {
                    regularExpression: /^(0?\.\d{1,15}|1\.[0-4]\d{0,14}|1\.5)$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setInertiaCoefFromString(text)) {
                        inertiaCoef.valid = true
                        root.setFlag(CheckList.InertiaCoef)
                    } else {
                        inertiaCoef.valid = false
                        root.clearFlag(CheckList.InertiaCoef)
                    }
                    root.valid = root.validate()
                }
            }
        }

        // CognitiveCoef 
        RowLayout {
            visible: (root.checkMask & CheckList.CognitiveCoef)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите значение когнитивного коэффициента (0.0-4.0)"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: cognitiveCoef
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "2.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^(0\.0\d{0,15}|0\.1)$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setCognitiveCoefFromString(text)) {
                        cognitiveCoef.valid = true
                        root.setFlag(CheckList.CognitiveCoef)
                    } else {
                        cognitiveCoef.valid = false
                        root.clearFlag(CheckList.CognitiveCoef)
                    }
                    root.valid = root.validate()
                }
            }
        }

        // SocialCoef 
        RowLayout {
            visible: (root.checkMask & CheckList.SocialCoef)
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Укажите значение социального коэффициента (0.0-4.0)"
            }

            Item { Layout.fillWidth: true }

            StyledTextField {
                id: socialCoef
                Layout.preferredWidth: 150
                boxed: true
                placeholderText: "2.0"

                validator: RegularExpressionValidator {
                    regularExpression: /^(0\.0\d{0,15}|0\.1)$/i
                }

                onActiveFocusChanged: {
                    if (acceptableInput
                        && inputData.setSocialCoefFromString(text)) {
                        socialCoef.valid = true
                        root.setFlag(CheckList.SocialCoef)
                    } else {
                        socialCoef.valid = false
                        root.clearFlag(CheckList.SocialCoef)
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
