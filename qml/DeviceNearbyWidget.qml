import QtQuick 2.15

import ThemeEngine 1.0

Rectangle {
    id: deviceNearbyWidget
    implicitWidth: 640
    implicitHeight: 48

    opacity: (device.deviceRssi < 0) ? 1 : 0.66
    color: (device.selected) ? Theme.colorForeground : Theme.colorBackground

    property var device: pointer
    property bool blacklisted: deviceManager.isBleDeviceBlacklisted(device.deviceAddress)

    Connections {
        target: deviceManager
        function onDevicesBlacklistUpdated() {
            deviceNearbyWidget.blacklisted = deviceManager.isBleDeviceBlacklisted(device.deviceAddress)
        }
    }

    MouseArea {
        anchors.fill: parent
        enabled: !singleColumn
        onClicked: device.selected = !device.selected
    }

    ////////////////////////////////////////////////////////////////////////////

    Column {
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        spacing: 0

        Text {
            id: deviceTitle
            text: device.deviceName
            textFormat: Text.PlainText
            font.pixelSize: Theme.fontSizeContent
            color: Theme.colorText
        }
        Text {
            text: device.deviceAddress
            textFormat: Text.PlainText
            font.pixelSize: Theme.fontSizeContentSmall
            color: Theme.colorSubText
            visible: !(Qt.platform.os === "osx" || Qt.platform.os === "ios")
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Row {
        anchors.right: parent.right
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        spacing: 12

        ////////

        Rectangle {
            id: barbg
            anchors.verticalCenter: parent.verticalCenter

            width: 128
            height: 16
            radius: 3
            color: Theme.colorSeparator

            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                width: parent.width * Math.abs(device.deviceRssi / 100)
                radius: 3
                color: {
                    if (device.deviceRssi < 0) {
                        if (device.deviceRssi > -65) return Theme.colorGreen
                        if (device.deviceRssi > -85) return Theme.colorOrange
                        if (device.deviceRssi > -100) return Theme.colorRed
                    } else {
                        if (device.deviceRssi < 65) return Theme.colorGreen
                        if (device.deviceRssi < 85) return Theme.colorOrange
                        if (device.deviceRssi < 100) return Theme.colorRed
                    }
                    return Theme.colorRed
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 4
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("-%1 dB").arg(Math.abs(device.deviceRssi))
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContentVerySmall
                    color: "white"
                }
            }

            //layer.enabled: false
            //layer.effect: OpacityMask {
            //    maskSource: Rectangle {
            //        x: barbg.x
            //        y: barbg.y
            //        width: barbg.width
            //        height: barbg.height
            //        radius: 4
            //    }
            //}
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 1
        color: Theme.colorSeparator
    }
}
