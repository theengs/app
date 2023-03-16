import QtQuick

import ThemeEngine 1.0

Rectangle {
    anchors.right: parent.right
    anchors.rightMargin: 16
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 16

    width: debugcol.width + 16
    height: debugcol.height + 16
    radius: 4
    opacity: 0.9
    color: Theme.colorForeground
    border.width: 1
    border.color: Theme.colorSubText
    visible: selectedDevice

    Column {
        id: debugcol
        anchors.centerIn: parent

        Text {
            text: "name : " + selectedDevice.deviceName
            color: Theme.colorSubText
        }
        Text {
            text: "model: " + selectedDevice.deviceModel
            color: Theme.colorSubText
        }
        Text {
            text: "type: " + selectedDevice.deviceType
            color: Theme.colorSubText
        }
    }
}
