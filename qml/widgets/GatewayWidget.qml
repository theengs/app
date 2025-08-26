import QtQuick

import ComponentLibrary
import DeviceUtils
import "qrc:/ComponentLibrary/UtilsNumber.js" as UtilsNumber
import "qrc:/js/UtilsDeviceSensors.js" as UtilsDeviceSensors

Item {
    id: gatewayWidget

    implicitWidth: 640
    implicitHeight: 128

    enabled: (boxDevice.available || boxDevice.connected)
    opacity: (boxDevice.available || boxDevice.connected) ? 1 : 0.66

    property var boxDevice: pointer

    property bool wideMode: ((width >= 380) || (isTablet && width >= 480))
    property bool hugeMode: (!isHdpi || (isTablet && width >= 480))
    property bool listMode: false

    Connections {
        target: boxDevice
        function onSensorUpdated() { initBoxData() }
        function onSensorsUpdated() { initBoxData() }
        function onCapabilitiesUpdated() { initBoxData() }
        function onStatusUpdated() { updateSensorStatus() }
        function onSettingsUpdated() { updateSensorSettings() }
        function onDataUpdated() { updateSensorData() }
    }
    Connections {
        target: Theme
        function onCurrentThemeChanged() {
            updateSensorSettings()
            updateSensorStatus()
            updateSensorData()
        }
    }
    Connections {
        target: settingsManager
        function onAppLanguageChanged() {
            updateSensorSettings()
            updateSensorStatus()
            updateSensorData()
        }
    }

    Component.onCompleted: initBoxData()

    ////////////////////////////////////////////////////////////////////////////

    function initBoxData() {
        updateSensorSettings()
        updateSensorStatus()
        updateSensorData()
    }

    function updateSensorStatus() {
        //textStatus.text = UtilsDeviceSensors.getDeviceStatusText(boxDevice.status)
        //textStatus.color = UtilsDeviceSensors.getDeviceStatusColor(boxDevice.status)
    }

    function updateSensorSettings() {
        // Title
        if (boxDevice.deviceModel) {
            textTitle.text = deviceManager.getDeviceModelTheengs(boxDevice.deviceModel)
        } else {
            textTitle.text = boxDevice.deviceName
        }
        // Location
        textLocation.font.pixelSize = hugeMode ? 20 : 18
        if (boxDevice.deviceLocationName) {
            textLocation.visible = true
            textLocation.text = boxDevice.deviceLocationName
        } else {
            if (Qt.platform.os === "osx" || Qt.platform.os === "ios") {
                textLocation.visible = false
                textLocation.text = ""
            } else {
                textLocation.visible = true
                textLocation.text = boxDevice.deviceAddress
            }
        }
    }

    function updateSensorData() {
        //
    }

    ////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: bottomSeparator
        height: 1
        anchors.left: parent.left
        anchors.leftMargin: -6
        anchors.right: parent.right
        anchors.rightMargin: -6
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        visible: listMode
        color: Theme.colorSeparator
    }

    Rectangle {
        anchors.fill: gatewayWidgetRectangle

        anchors.leftMargin: listMode ? -12 : 0
        anchors.rightMargin: listMode ? -12 : 0
        anchors.topMargin: listMode ? -6 : 0
        anchors.bottomMargin: listMode ? -6 : 0

        radius: 4
        border.width: 2
        border.color: listMode ? "transparent" : Theme.colorSeparator

        color: boxDevice.selected ? Theme.colorSeparator : Theme.colorgatewayWidget
        Behavior on color { ColorAnimation { duration: 133 } }

        opacity: boxDevice.selected ? 0.5 : (listMode ? 0 : 1)
        Behavior on opacity { OpacityAnimator { duration: 133 } }
    }

    ////////////////////////////////////////////////////////////////////////////

    Item {
        id: gatewayWidgetRectangle

        anchors.fill: parent
        anchors.margins: 6

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.MiddleButton

            onClicked: (mouse) => {
                if (typeof boxDevice === "undefined" || !boxDevice) return

                if (mouse.button === Qt.LeftButton) {
                    // multi selection

                    // regular click
                    selectedGateway = boxDevice
                    screenGatewayDevice.loadDevice(boxDevice)
                }

                if (mouse.button === Qt.MiddleButton) {
                   // multi selection
                   return
                }
            }

            onPressAndHold: {
                // multi selection
            }
        }

        ////////////////

        Row {
            id: rowLeft

            anchors.top: parent.top
            anchors.topMargin: hugeMode ? 16 : 8
            anchors.left: parent.left
            anchors.leftMargin: hugeMode ? (listMode ? 4 : 16) : (listMode ? 6 : 14)
            anchors.right: rowRight.left
            anchors.rightMargin: listMode ? 0 : 8
            anchors.bottom: parent.bottom
            anchors.bottomMargin: hugeMode ? 16 : 8

            spacing: hugeMode ? (listMode ? 20 : 12) : (listMode ? 24 : 10)

            IconSvg {
                id: imageDevice
                width: hugeMode ? 32 : 24
                height: hugeMode ? 32 : 24
                anchors.verticalCenter: parent.verticalCenter

                source: boxDevice.onboarded ?
                            "qrc:/IconLibrary/material-symbols/build_circle-fill.svg" :
                            "qrc:/IconLibrary/material-symbols/build_circle.svg"
                color: boxDevice.onboarded ? Theme.colorGreen : Theme.colorHighContrast
                visible: (wideMode || hugeMode)
                fillMode: Image.PreserveAspectFit
                asynchronous: true

                //TagClear { // DEBUG
                //    anchors.horizontalCenter: parent.horizontalCenter
                //    anchors.top: parent.bottom
                //    anchors.topMargin: 8

                //    color: Theme.colorGreen
                //    text: boxDevice.onboardingStatus
                //}
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    id: textTitle
                    width: rowLeft.width - imageDevice.width - rowLeft.spacing

                    textFormat: Text.PlainText
                    color: Theme.colorText
                    font.pixelSize: hugeMode ? 22 : 20
                    //font.capitalization: Font.Capitalize
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight

                    ItemBadge { // DEBUG
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: textTitle.contentWidth + 8

                        color: Theme.colorGrey
                        text: boxDevice.onboardingStatus
                    }
                }

                Text {
                    id: textLocation
                    width: rowLeft.width - imageDevice.width - rowLeft.spacing

                    textFormat: Text.PlainText
                    color: Theme.colorSubText
                    font.pixelSize: hugeMode ? 20 : 18
                    //font.capitalization: Font.Capitalize
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                Row {
                    id: row
                    height: hugeMode ? 26 : 22
                    anchors.left: parent.left
                    spacing: 8

                    Text {
                        id: textStatus
                        anchors.verticalCenter: parent.verticalCenter

                        textFormat: Text.PlainText
                        color: Theme.colorGreen
                        font.pixelSize: hugeMode ? 16 : 15

                        SequentialAnimation on opacity {
                            id: opa
                            loops: Animation.Infinite
                            alwaysRunToEnd: true
                            running: (visible &&
                                      boxDevice.status !== DeviceUtils.DEVICE_OFFLINE &&
                                      boxDevice.status !== DeviceUtils.DEVICE_QUEUED &&
                                      boxDevice.status !== DeviceUtils.DEVICE_CONNECTED)

                            PropertyAnimation { to: 0.33; duration: 750; }
                            PropertyAnimation { to: 1; duration: 750; }
                        }
                    }
/*
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
                            width: parent.width * Math.abs(boxDevice.rssi / 100)
                            radius: 3
                            color: {
                                if (Math.abs(boxDevice.rssi) < 65) return Theme.colorGreen
                                if (Math.abs(boxDevice.rssi) < 85) return Theme.colorOrange
                                if (Math.abs(boxDevice.rssi) < 100) return Theme.colorRed
                                return Theme.colorRed
                            }

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 4
                                anchors.verticalCenter: parent.verticalCenter

                                text: qsTr("-%1 dB").arg(Math.abs(boxDevice.rssi))
                                textFormat: Text.PlainText
                                font.pixelSize: Theme.fontSizeContentVerySmall
                                color: "white"
                            }
                        }
                    }
*/
                    ProgressChip {
                        height: 28
                        progress: Math.abs(boxDevice.rssiMean)

                        // signal_wifi_0_bar.svg // wifi_channel-fill.svg
                        leftIcon: "qrc:/IconLibrary/material-symbols/bluetooth.svg"
                        color: {
                            if (Math.abs(boxDevice.rssiMean) < 65) return Theme.colorGreen
                            if (Math.abs(boxDevice.rssiMean) < 85) return Theme.colorOrange
                            if (Math.abs(boxDevice.rssiMean) < 100) return Theme.colorRed
                            return Theme.colorRed
                        }
                    }

                    ButtonChip {
                        height: 28
                        text: qsTr("wifi")

                        leftIcon: {
                            if (boxDevice && boxDevice.onboardingStatus >= 3) return "qrc:/IconLibrary/material-symbols/signal_wifi_4_bar.svg"
                            return "qrc:/IconLibrary/material-symbols/signal_wifi_off.svg"
                        }
                        color: {
                            if (boxDevice && boxDevice.onboardingStatus >= 3) return Theme.colorGreen
                            return Theme.colorGrey
                        }
                    }

                    ButtonChip {
                        height: 28
                        text: qsTr("broker")

                        leftIcon: {
                            if (boxDevice && boxDevice.onboardingStatus >= 4) return "qrc:/IconLibrary/material-symbols/wifi_tethering.svg"
                            return "qrc:/IconLibrary/material-symbols/wifi_tethering_off.svg"
                        }
                        color: {
                            if (boxDevice && boxDevice.onboardingStatus >= 4) return Theme.colorBlue
                            return Theme.colorGrey
                        }
                    }
                }
            }
        }

        ////////////////

        Row {
            id: rowRight

            anchors.top: parent.top
            anchors.topMargin: hugeMode ? 16 : 8
            anchors.bottom: parent.bottom
            anchors.bottomMargin: hugeMode ? 16 : 8
            anchors.right: parent.right
            anchors.rightMargin: listMode ? (wideMode ? 0 : -4) : (hugeMode ? 14 : 10)
            spacing: 8
/*
            IconSvg {
                id: imageStatus
                anchors.verticalCenter: parent.verticalCenter
                width: 32
                height: 32

                opacity: 0.8
                color: Theme.colorIcon

                source: {
                    if (boxDevice.status >= DeviceUtils.DEVICE_CONNECTED) {
                        return "qrc:/IconLibrary/material-symbols/bluetooth_connected.svg"
                    } else if (boxDevice.available) { // (boxDevice.status === DeviceUtils.DEVICE_AVAILABLE) {
                        return "qrc:/IconLibrary/material-symbols/bluetooth.svg"
                    }
                    return "" // "qrc:/IconLibrary/material-symbols/bluetooth_disabled.svg"
                }
            }
*/
            IconSvg {
                id: imageForward
                anchors.verticalCenter: parent.verticalCenter
                width: 32
                height: 32

                visible: listMode
                color: boxDevice.hasData ? Theme.colorHighContrast : Theme.colorSubText
                source: "qrc:/IconLibrary/material-symbols/chevron_right.svg"
            }
        }

        ////////////////
    }

    ////////////////////////////////////////////////////////////////////////////
}
