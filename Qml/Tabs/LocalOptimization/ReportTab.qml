pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AppEnums
import LOInputData
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

    property int checkMask: helper.getCheckByFullType(inputData.fullAlgoId)

    function buildTaskDescription() {
        var fn = "<не задана>";
        if (inputData && inputData.function) {
            fn = inputData.function;
        }

        var extremum = helper.extremumTypeToString(inputData.extremumId)
        if (extremum) extremum = extremum.toLowerCase()

        var algo = helper.algoTypeToString(inputData.algorithmId);
        if (algo) algo = algo.toLowerCase();

        var extensionPart = "";
        if (inputData.extensionId !== ExtensionType.B) {
            var ext = helper.extensionTypeToString(inputData.extensionId);
            if (ext) ext = ext.toLowerCase();
            extensionPart = " и его расширение — " + ext;
        }

        return "Найти " + extremum + " функции " + fn +
               ", применяя " + (algo || "") + extensionPart +
               ", за не более чем " + inputData.maxIterations +
               " шагов и не более чем " + inputData.maxFuncCalls +
               " вызовов функции, с точностью вычислений, равной 10^-" +
               inputData.calcAccuracy +
               ", и с учетом заданного набора параметров: ";
    }
    
    function stepTypeDescription() {
        var stepstr = helper.stepTypeToString(inputData.stepId);
        return "— тип шага — " + stepstr.toLowerCase() + ";";
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
            flickableDirection: Flickable.HorizontalAndVerticalFlick
            flickDeceleration: 0.0000001

            clip: true

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
                    visible: ((root.checkMask !== CheckList.GDSCheck)
                        && (root.checkMask !== CheckList.GDRCheck))
                    Layout.preferredWidth: flickable.width
                    text: stepTypeDescription()
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.StartX1)
                    Layout.preferredWidth: flickable.width
                    text: "— начальное приближение по X — " + root.inputData.startX1 + ";"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.StartY1)
                    text: "— начальное приближение по Y — " + root.inputData.startY1 + ";"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.StartX2)
                    Layout.preferredWidth: flickable.width
                    text: "— второе начальное приближение по X — " + root.inputData.startX2 + ";"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.StartY2)
                    text: "— второе начальное приближение по Y — " + root.inputData.startY2 + ";"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.StepX)
                    Layout.preferredWidth: flickable.width
                    text: "— шаг по X — " + root.inputData.stepX + ";"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.StepY)
                    text: "— шаг по Y — " + root.inputData.stepY + ";"
                    font.pixelSize: root.fontSize
                    wrapMode: Text.WordWrap
                }

                Text {
                    visible: (root.checkMask & CheckList.Step)
                    text: "— шаг — " + root.inputData.step + ";"
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

                    delegate: Item {
                        id: container

                        required property string type
                        required property string title
                        required property string text
                        required property double value
                        required property var columns
                        required property var rows

                        implicitWidth: (valueLoader.active 
                            ? valueLoader.implicitWidth : messageLoader.active
                                ? messageLoader.implicitWidth : flickable.width)
                        implicitHeight: (valueLoader.active
                            ? valueLoader.implicitHeight : messageLoader.active
                                ? messageLoader.implicitHeight : tableLoader.implicitHeight)

                        Loader {
                            id: valueLoader
                            active: (type === "value")

                            sourceComponent: Text {
                                text: container.title + ": " + container.value
                                font.pixelSize: root.fontSize
                                wrapMode: Text.WordWrap
                            }
                        }

                        Loader {
                            id: messageLoader
                            active: (type === "message")

                            sourceComponent: Text {
                                width: flickable.width
                                text: container.text
                                font.pixelSize: root.fontSize
                                wrapMode: Text.WordWrap
                            }
                        }

                        TableLoader {
                            id: tableLoader
                            active: (type === "table")

                            anchors.centerIn: parent

                            title: container.title
                            columns: container.columns
                            rows: container.rows

                            fontSize: root.fontSize
                        }
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
