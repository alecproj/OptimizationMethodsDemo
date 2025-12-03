pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AppEnums
import GOInputData
import Globals
import Components

Rectangle {
    id: root
    color: AppPalette.background

    required property string fileName
    required property var inputData
    required property var solution
    required property var resultData
    property int fontSize: 14

    EnumHelper { id: helper }

    property int checkMask: helper.getCheckByAlgoType(inputData.algorithmId)

    function buildTaskDescription() {
        var fn = "<не задана>";
        if (inputData && inputData.function) {
            fn = inputData.function;
        }

        var extremum = helper.extremumTypeToString(inputData.extremumId)
        if (extremum) extremum = extremum.toLowerCase()

        var algo = helper.algoTypeToString(inputData.algorithmId);
        if (algo) algo = algo.toLowerCase();

        var calc = inputData.calcAccuracy;
        if (inputData.algorithmId === AlgoType.GA) {
            calc = ", зависящей от длины кода, равной " + calc;
        } else if (inputData.algorithmId === AlgoType.PS) {
            calc = ", равной 10^-" + calc;
        }

        return "Найти глобальный " + extremum + " функции " + fn +
               ", применяя " + (algo || "") +
               ", за не более чем " + inputData.maxIterations +
               " итераций, с точностью вычислений" + calc +
               ", и с учетом заданного набора параметров: ";
    }
    
    function buildAnswerDescription() {
        if (resultData) {
            var x = resultData.xValue;
            var y = resultData.yValue;
            var func = resultData.funcValue;
            return "Ответ: x = " + x + ", y = " + y + ", f(x,y) = " + func + ".";
        }
        return "Ответ не найден."
    }

    ColumnLayout {
        id: content
        width: (parent.width * 0.9)
        height: parent.height
        anchors.centerIn: parent
        spacing: 10

        Flickable {
            id: flickable
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: column.implicitWidth
            contentHeight: column.implicitHeight

            clip: true
            flickableDirection: Flickable.AutoFlickIfNeeded

            ColumnLayout {
                id: column
                anchors.fill: parent

                spacing: 5

                Label {
                    id: title

                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: column.spacing

                    text: "Отчет " + root.fileName
                    font.pixelSize: 20
                    font.bold: true
                }

                // ----------------- TASK ----------------

                Text {
                    text: "Задача"
                    topPadding: 10
                    font.pixelSize: 16
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    Layout.preferredWidth: flickable.width
                    text: root.buildTaskDescription()
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.Size)
                        && (root.inputData.algorithmId === AlgoType.PS)
                    Layout.preferredWidth: flickable.width
                    text: "— Размер роя — " + root.inputData.size + " частиц;"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.Size)
                        && (root.inputData.algorithmId === AlgoType.GA)
                    Layout.preferredWidth: flickable.width
                    text: "— размер популяции — " + root.inputData.size + " особей;"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.Elitism)
                    text: "— количество элитных особей — " + root.inputData.elitism + ";"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.CrossoverProb)
                    text: "— вероятность скрещивания — " + root.inputData.crossoverProb+ ";"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.MutationProb)
                    text: "— вероятность мутации — " + root.inputData.mutationProb + ";"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.InertiaCoef)
                    text: "— коэффициент инерции — " + root.inputData.inertiaCoef + ";"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.CognitiveCoef)
                    text: "— когнитивный коэффициент — " + root.inputData.cognitiveCoef + ";"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.SocialCoef)
                    text: "— социальный коэффициент — " + root.inputData.socialCoef + ";"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.MinX) && (root.checkMask & CheckList.MaxX)
                    text: "— диапазон по X — [" + root.inputData.minX + ";" + root.inputData.maxX + "];"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.MinY) && (root.checkMask & CheckList.MaxY)
                    text: "— диапазон по Y — [" + root.inputData.minY + ";" + root.inputData.maxY + "]."
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    text: "Результат представить с точностью: 10^-" + root.inputData.resultAccuracy + "."
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                // --------------- SOLUTION --------------
                Text {
                    text: "Решение"
                    topPadding: 10
                    font.pixelSize: 16
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Repeater {
                    model: root.solution

                    delegate: SolutionDelegate {
                        Layout.fillWidth: true
                    }
                } // Repeater

                Text {
                    Layout.topMargin: 10
                    text: root.buildAnswerDescription()
                    font.pixelSize: root.fontSize
                    font.bold: true
                    wrapMode: Text.WordWrap
                }
            } // column

            ScrollBar.vertical: ScrollBar{ 
                id: vScroll
                policy: (flickable.contentHeight > flickable.height) ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
            }
        } // flickable


        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: 10
            spacing: 10

            Item {Layout.fillWidth: true}

            StyledButton {
                id: closeBtn
                text: "Закрыть"
                onReleased: {
                    controller.closeReport(root.fileName)
                    AppStates.currentTabIndex = 1;
                }
            }

            StyledButton {
                id: deleteBtn
                text: "Удалить"
                bgcolor: AppPalette.warning
                onReleased: {
                    controller.requestDeleteReport(root.fileName)
                }
            }
        }

    } // content
}
