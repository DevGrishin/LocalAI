import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    width: parent.width
    height: 60
    color: isSelected ? "#3d3c4f" : "transparent"
    radius: 8

    property int conversationId: -1
    property string conversationName: ""
    property bool isSelected: false
    property bool isEditing: false
    property bool showButtons: false

    signal selectConversation(int id)
    signal deleteConversation(int id)
    signal renameConversation(int id, string newName)

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 8


        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                id: nameText
                anchors.verticalCenter: parent.verticalCenter
                text: conversationName
                color: "#d1d1d1"
                font.pixelSize: 16
                visible: !isEditing
                elide: Text.ElideRight
                width: parent.width
            }

            TextField {
                id: nameInput
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                text: conversationName
                visible: isEditing
                font.pixelSize: 16
                color: "#d1d1d1"
                background: Rectangle {
                    color: "#2a293b"
                    border.color: "#5a5a5a"
                    border.width: 1
                    radius: 4
                }

                Keys.onPressed: (event) => {
                    if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                        finishEditing()
                        event.accepted = true
                    } else if (event.key === Qt.Key_Escape) {
                        cancelEditing()
                        event.accepted = true
                    }
                }

                onActiveFocusChanged: {
                    if (!activeFocus && isEditing) {
                        finishEditing()
                    }
                }
            }
        }


        Rectangle {
            id: editButton
            Layout.preferredWidth: 32
            Layout.preferredHeight: 32
            visible: showButtons || isEditing
            color: editMouseArea.pressed ? "#5a5a5a" : editMouseArea.containsMouse ? "#4a4a4a" : "transparent"
            radius: 16

            Text {
                anchors.centerIn: parent
                text: isEditing ? "✓" : "✎"
                color: "#d1d1d1"
                font.pixelSize: 14
            }

            MouseArea {
                id: editMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    if (isEditing) {
                        finishEditing()
                    } else {
                        startEditing()
                    }
                }
            }
        }


        Rectangle {
            id: deleteButton
            Layout.preferredWidth: 32
            Layout.preferredHeight: 32
            visible: showButtons && !isEditing
            color: deleteMouseArea.pressed ? "#8a4a4a" : deleteMouseArea.containsMouse ? "#7a4a4a" : "transparent"
            radius: 16

            Text {
                anchors.centerIn: parent
                text: "🗑"
                color: "#ff6b6b"
                font.pixelSize: 14
            }

            MouseArea {
                id: deleteMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    root.deleteConversation(conversationId)
                }
            }
        }
    }
    MouseArea {
        anchors.fill: parent
        z: 1000
        hoverEnabled: true
        acceptedButtons: Qt.NoButton

        onContainsMouseChanged: {
            root.showButtons = containsMouse
        }
    }

    MouseArea {
        anchors.fill: parent
        z: -1
        onClicked: {
            if (!isEditing) {
                root.selectConversation(conversationId)
            }
        }
    }

    function startEditing() {
        isEditing = true
        nameInput.forceActiveFocus()
        nameInput.selectAll()
    }

    function finishEditing() {
        if (nameInput.text.trim() !== "" && nameInput.text !== conversationName) {
            root.renameConversation(conversationId, nameInput.text.trim())
        }
        isEditing = false
    }

    function cancelEditing() {
        nameInput.text = conversationName
        isEditing = false
    }
}
