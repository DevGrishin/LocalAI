import QtQuick
import QtQuick.Controls

import QtQuick 2.15
import QtQuick.Controls 2.15


TextEdit {
    text: ""
    textFormat: Text.RichText
    wrapMode: Text.Wrap
    readOnly: true
    selectByMouse: true
    color: "white"
    font.pixelSize: 30
    width: parent ? parent.width : 400
}
