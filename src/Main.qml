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
                contentHeight: historyList.height
                contentWidth: historyList.width

                Column {
                    id: historyList
                    x: 0
                    y: 0
                    width: 405
                    spacing: 8
                    objectName: "historyList"

                    function clear() {
                        console.log("Clearing history list...");
                        for (var i = children.length - 1; i >= 0; i--) {
                            if (children[i].objectName === "historyItem") {
                                children[i].destroy();
                            }
                        }
                    }

                    function addHistoryItem(itemData) {
                        console.log("Adding history item:", itemData.conversationId, itemData.conversationName);
                        var component = Qt.createComponent("HistoryItem.qml");
                        if (component.status === Component.Ready) {
                            var item = component.createObject(historyList, {
                                "conversationId": itemData.conversationId,
                                "conversationName": itemData.conversationName,
                                "isSelected": itemData.isSelected,
                                "objectName": "historyItem"
                            });

                            if (item) {
                                item.selectConversation.connect(function(id) {
                                    myHandler.selectConversation(id);
                                });

                                item.deleteConversation.connect(function(id) {
                                    myHandler.deleteConversation(id);
                                });

                                item.renameConversation.connect(function(id, newName) {
                                    myHandler.renameConversation(id, newName);
                                });

                                console.log("History item created successfully");
                            } else {
                                console.error("Failed to create history item");
                            }
                        } else {
                            console.error("HistoryItem component not ready:", component.errorString());
                        }
                    }
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

                function down(){
                    ScrollBar.position = 1;
                }



                property var lastItem: null

                function clear() {
                    for (var i = children.length - 1; i >= 0; i--) {
                        children[i].destroy();
                    }
                    lastItem = null;
                }

                function addItem(message) {
                    var component = Qt.createComponent("Label.qml");
                    if (component.status === Component.Ready) {
                        var item = component.createObject(logColumn, {"text": message});
                        if (item === null) {
                            console.error("Failed to create item");
                        } else {
                            lastItem = item;
                        }
                    } else {
                        console.error("Component not ready:", component.errorString());
                    }
                }

                function updateLastItem(newText) {
                    if (lastItem) {
                        lastItem.text = newText;
                    }
                }
            }
        }

        Button {
            id: newChatButton
            x: 319
            y: 23
            height: 40
            width: 100
            text: qsTr("New Chat")
            onClicked: {
                myHandler.createNewConversation();
            }
        }

        RoundButton {
            id: roundButton
            x: 1696
            y: 972
            width: 58
            height: 58
            text: "stop"
            onClicked: myHandler.stop = false
        }
    }

    Connections {
        target: myHandler
        function onCurrentConversationChanged() {
            console.log("Current conversation changed");
        }

        function onConversationHistoryChanged() {
            console.log("Conversation history updated");
        }
    }
}
