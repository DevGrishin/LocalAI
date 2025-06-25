import QtQuick
import QtQuick.Controls
import QtQuick3D 6.8



Window {
    width: 1920
    height: 1080
    visible: true
    title: "LocalAI"

    Rectangle {
        id: bgMain
        width: 1920
        height: 1080
        color: "#2a293b"

        Rectangle {
            id: bgSideBar
            x: 0
            y: 0
            width: 438
            height: 1080
            color: "#292836"

            Text {
                id: historyLabel
                x: 25
                y: 68
                color: "#d1d1d1"
                text: qsTr("History")
                font.pixelSize: 40
            }

            ScrollView {
                id: historyScroll
                x: 25
                y: 158
                width: 405
                height: 873
                contentHeight: historyList.width
                contentWidth: historyList.implicitHeight

                Column {
                    id: historyList
                    x: 0
                    y: 0
                    width: 405
                    height: 873
                }
            }
        }

        Rectangle {
            id: bgUserInput
            x: 710
            y: (height < 200) ? 1020 - userInputScroll.implicitContentHeight : 1020 - 190
            width: 945
            height: (10 + userInputScroll.implicitContentHeight
                     <= 200) ? 10 + userInputScroll.implicitContentHeight : 200
            color: "#323142"
            radius: 38

            ScrollView {
                id: userInputScroll
                x: 35
                y: bgUserInput.anchors.topMargin
                width: 875
                height: bgUserInput.height - 10
                contentHeight: userInputText.contentHeight + 12


                TextArea {
                    id: userInputText
                    x: 0
                    y: bgUserInput.anchors.bottomMargin
                    width: 875
                    height: 58
                    color: "#d1d1d1"
                    text: ""
                    wrapMode: Text.Wrap
                    placeholderTextColor: "#a9d1d1d1"
                    font.pointSize: 20
                    placeholderText: qsTr("Enter prompt")
                    background: Rectangle {
                            color: "transparent"
                    }

                    Keys.onPressed: (event) => {
                        if ((event.key === Qt.Key_Enter || event.key === Qt.Key_Return) & myHandler.available) {
                            if (!(event.modifiers & Qt.ShiftModifier)){
                                myHandler.onEnterPressed(text);
                                text = "";
                                event.accepted = true;
                            }
                        }
                    }
                }
            }
        }

        ComboBox {
            id: modelCombo
            x: 455
            y: 23
            width: 291
            height: 40
            opacity: 1
            wheelEnabled: false
            font.pointSize: 21
            displayText: "Loading Models..."
            flat: false
            model: myHandler.comboOptions
            onCurrentTextChanged: {
                displayText = currentText;
                myHandler.onComboSelectionChanged(currentText)
            }

        }
        Rectangle {
            id: bgSettingsPanel
            x: 1705
            y: 8
            width: 200
            height: 200
            visible: false
            color: "#ffffff"
        }
        RoundButton {
            id: settingsButton
            x: 1842
            y: 8
            width: 70
            height: 70
            checkable: true
            display: AbstractButton.IconOnly
            flat: true
            icon.color: "#1a1c1e"
            highlighted: false
            icon.height: 170
            icon.width: 170
            icon.source: "cog.png"
        }

        ScrollView {
            id: chatScroll
            x: 463
            y: 98
            width: 1441
            height: 852

            Column {
                id: logColumn
                x: 0
                y: 0
                width: 1417
                height: 852
                spacing: 15
                objectName: "logColumn"

                property var lastItem: null

                function addItem(message) {
                    var component = Qt.createComponent("Label.qml");
                    if (component.status === Component.Ready) {
                        var item = component.createObject(logColumn, {"text": message});
                        if (item === null) {
                            console.error("Failed to create item");
                        } else {
                            lastItem = item; // store reference to last added item
                        }
                    } else {
                        console.error("Component not ready:", component.errorString());
                    }
                }

                // Then later you can update lastItem's properties, e.g.
                function updateLastItem(newText) {
                    if (lastItem) {
                        lastItem.text = newText;
                    }
                }
            }
        }

        states: [
            State {
                name: "Settings"
                when: settingsButton.checked

                PropertyChanges {
                    target: bgSettingsPanel
                    x: 1316
                    y: 8
                    width: 596
                    height: 270
                    visible: true
                    color: "#323142"
                    radius: 34
                }

                PropertyChanges {
                    target: bgUserInput
                    radius: 29
                }
            }
        ]
    }


}
