import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import ComponentLibrary
import DeviceUtils
import "qrc:/js/UtilsDeviceSensors.js" as UtilsDeviceSensors

Loader {
    id: deviceActuator

    property var currentDevice: null

    ////////

    function loadDevice(clickedDevice) {
        if (typeof clickedDevice === "undefined" || !clickedDevice) return
        if (!clickedDevice.isActuatorWindow) return

        // set device
        if (currentDevice !== clickedDevice) currentDevice = clickedDevice

        // connect device
        currentDevice.actionConnect(true)

        // load screen
        if (!deviceActuator.active) deviceActuator.active = true
        deviceActuator.item.loadDevice()

        // change screen
        appContent.state = "DeviceActuatorWindow"
    }

    ////////

    function backAction() {
        if (deviceActuator.status === Loader.Ready)
            deviceActuator.item.backAction()

        if (currentDevice)
            currentDevice.actionDisconnect()
    }

    ////////////////////////////////////////////////////////////////////////////

    active: false
    asynchronous: false

    sourceComponent: Item {
        id: itemDeviceActuator
        implicitWidth: 480
        implicitHeight: 720

        focus: parent.focus

        property string cccc: headerUnicolor ? Theme.colorHeaderContent : "white"

        ////////

        Connections {
            target: currentDevice
            function onSensorUpdated() { updateHeader() }
            function onSensorsUpdated() { updateHeader() }
            function onCapabilitiesUpdated() { updateHeader() }
            function onStatusUpdated() { updateHeader() }
            function onDataUpdated() {
                updateData()
            }
            function onRefreshUpdated() {
                updateData()
                updateGraph()
            }
            function onHistoryUpdated() {
                updateGraph()
            }
        }

        Connections {
            target: settingsManager
            function onTempUnitChanged() {
                updateData()
            }
            function onAppLanguageChanged() {
                updateData()
                updateStatusText()
            }
            function onGraphThermometerChanged() {
                loadGraph()
            }
        }

        Connections {
            target: appHeader
            // desktop only
            function onDeviceDataButtonClicked() {
                appHeader.setActiveDeviceData()
            }
            function onDeviceSettingsButtonClicked() {
                appHeader.setActiveDeviceSettings()
            }
            // mobile only
            function onRightMenuClicked() {
                //
            }
        }

        Timer {
            interval: 60000; running: true; repeat: true;
            onTriggered: updateStatusText()
        }

        Keys.onPressed: (event) => {
            if (event.key === Qt.Key_F5) {
                event.accepted = true
                deviceManager.updateDevice(currentDevice.deviceAddress)
            } else if (event.key === Qt.Key_Backspace) {
                event.accepted = true
                appWindow.backAction()
            }
        }

        ////////

        function loadDevice() {
            //console.log("deviceActuator // loadDevice() >> " + currentDevice)

            loadGraph()
            updateHeader()
            updateData()
        }

        function updateHeader() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isActuatorWindow) return
            //console.log("deviceActuator // updateHeader() >> " + currentDevice)

            updateStatusText()
        }

        function updateData() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isActuatorWindow) return
            //console.log("deviceActuator // updateData() >> " + currentDevice)
        }

        function updateStatusText() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isActuatorWindow) return
            //console.log("deviceActuator // updateStatusText() >> " + currentDevice)

            textStatus.text = UtilsDeviceSensors.getDeviceStatusText(currentDevice.status)
        }

        function loadGraph() {
            //
        }
        function updateGraph() {
            //
        }

        ////////

        function backAction() {
            if (textInputLocation.focus) {
                textInputLocation.focus = false
                return
            }
            if (isHistoryMode()) {
                resetHistoryMode()
                return
            }

            appContent.state = "DeviceList"
        }

        function isHistoryMode() {
            return false
        }
        function resetHistoryMode() {
            //
        }

        ////////////////////////////////////////////////////////////////////////

        Flow {
            anchors.fill: parent

            Rectangle {
                id: actuatorBox

                property int dimboxw: Math.min(deviceActuator.width * 0.4, isPhone ? 300 : 600)
                property int dimboxh: Math.max(deviceActuator.height * 0.333, isPhone ? 180 : 256)

                width: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return parent.width
                    return singleColumn ? parent.width : dimboxw
                }
                height: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return dimboxh
                    return singleColumn ? dimboxh : parent.height
                }

                color: Theme.colorHeader
                z: 5

                MouseArea { anchors.fill: parent } // prevent clicks below this area

                Item { // square indicator (mimic a window)
                    width: singleColumn ? actuatorBox.height * 0.75 : actuatorBox.width * 0.66
                    height: width

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.height / 3)

                    Rectangle {
                        id: indicator
                        anchors.fill: parent
                        anchors.margins: 12

                        radius: 4
                        color: Qt.alpha(cccc, 0.1)
                        border.width: 2
                        border.color: Qt.alpha(cccc, 0.33)

                        rotation: (currentDevice.deviceModel === "W270160X") ? 90 : 0

                        Rectangle {
                            id: indicatorSlider
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.bottom: parent.bottom
                            anchors.margins: 2

                            radius: 2
                            opacity: 0.33
                            width: (indicator.width-4) * (currentDevice.position/100.0)
                            color: Theme.colorPrimary
                        }
                    }

                    IconSvg { // sensorDisconnected
                        width: isMobile ? 96 : 128
                        height: isMobile ? 96 : 128
                        anchors.centerIn: parent

                        visible: !(currentDevice.available || currentDevice.connected) // !currentDevice.hasData
                        source: "qrc:/IconLibrary/material-symbols/bluetooth_disabled.svg"
                        color: cccc
                    }

                    Column {
                        anchors.centerIn: parent

                        visible: (currentDevice.available || currentDevice.connected) // currentDevice.hasData
                        spacing: 0

                        Text { // legend
                            anchors.horizontalCenter: parent.horizontalCenter

                            text: {
                                return currentDevice.direction

                                if (currentDevice.hasOpen) return "open"
                                else if (currentDevice.hasMotion) return "motion"
                                else return ""
                            }

                            font.pixelSize: isPhone ? 22 : 26
                            font.bold: false
                            color: cccc
                            opacity: 0.66
                        }

                        Text { // value
                            anchors.horizontalCenter: parent.horizontalCenter

                            text: {
                                return currentDevice.position
                                //return "?"
                            }

                            font.pixelSize: isPhone ? 26 : 30
                            font.bold: true
                            color: cccc
                            opacity: 1
                        }

                        Item {
                            width: 12
                            height: 12
                            //visible: (currentDevice.hasLuminositySensor && currentDevice.luminosityLux >= 0)
                        }

                        Row {
                            anchors.horizontalCenter: parent.horizontalCenter
                            spacing: 8
                            //visible: (currentDevice.hasLuminositySensor && currentDevice.luminosityLux >= 0)

                            Text {
                                text: currentDevice.lightlevel
                                font.pixelSize: isPhone ? 22 : 24
                                color: cccc
                                opacity: 1
                            }
                            Text {
                                text: "lux"
                                font.pixelSize: isPhone ? 20 : 22
                                color: cccc
                                opacity: 0.66
                            }
                        }

                        IconSvg {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: 30
                            height: 32

                            visible: (currentDevice.hasBattery && currentDevice.deviceBattery >= 0)
                            source: UtilsDeviceSensors.getDeviceBatteryIcon(currentDevice.deviceBattery)
                            color: cccc
                            rotation: 90
                            fillMode: Image.PreserveAspectCrop
                        }
                    }
                }

                ////////

                Row {
                    id: status
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.right: itemLocation.left
                    anchors.rightMargin: 8
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 8

                    height: 24
                    spacing: 8

                    IconSvg {
                        id: imageStatus
                        width: 24
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter

                        source: "qrc:/IconLibrary/material-icons/duotone/schedule.svg"
                        color: cccc
                    }
                    Text {
                        id: textStatus
                        width: status.width - status.spacing - imageStatus.width
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("Loading…")
                        color: cccc
                        font.bold: false
                        font.pixelSize: 17
                        elide: Text.ElideRight
                    }
                }

                ////////

                Row {
                    id: itemLocation
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 8
                    anchors.right: parent.right
                    anchors.rightMargin: 8

                    height: 24
                    spacing: 4

                    IconSvg {
                        id: imageEditLocation
                        width: 20
                        height: 20
                        anchors.verticalCenter: parent.verticalCenter

                        source: "qrc:/IconLibrary/material-icons/duotone/edit.svg"
                        color: cccc

                        opacity: (isMobile || !textInputLocation.text || textInputLocation.focus || textInputLocationArea.containsMouse) ? 0.9 : 0
                        Behavior on opacity { OpacityAnimator { duration: 133 } }
                    }
                    TextInput {
                        id: textInputLocation
                        anchors.verticalCenter: parent.verticalCenter

                        padding: 4
                        font.pixelSize: 17
                        font.bold: false
                        color: cccc

                        text: currentDevice ? currentDevice.deviceLocationName : ""
                        onEditingFinished: {
                            currentDevice.deviceLocationName = text
                            focus = false
                        }

                        MouseArea {
                            id: textInputLocationArea
                            anchors.fill: parent
                            anchors.topMargin: -4
                            anchors.leftMargin: -24
                            anchors.rightMargin: -4
                            anchors.bottomMargin: -4

                            hoverEnabled: true
                            propagateComposedEvents: true

                            onPressed: (mouse) => {
                                textInputLocation.forceActiveFocus()
                                mouse.accepted = false
                            }
                        }
                    }
                    IconSvg {
                        id: imageLocation
                        width: 24
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter

                        source: "qrc:/IconLibrary/material-icons/duotone/pin_drop.svg"
                        color: cccc
                    }
                }

                ////////

                Rectangle {
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    visible: ((isDesktop || headerUnicolor) && !singleColumn)
                    width: 2
                    opacity: 0.33
                    color: Theme.colorHeaderHighlight
                }
                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    visible: ((isDesktop || headerUnicolor) && singleColumn)
                    height: 2
                    opacity: 0.33
                    color: Theme.colorHeaderHighlight
                }
            }

            ////////////////////////////////////////////////////////////////////

            Item {
                width: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return parent.width
                    return singleColumn ? parent.width : (parent.width - actuatorBox.width)
                }
                height: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return (parent.height - actuatorBox.height)
                    return singleColumn ? (parent.height - actuatorBox.height) : parent.height
                }

                // EMPTY

                ////////////////

                Rectangle {
                    id: infoArea
                    anchors.left: parent.left
                    anchors.right: parent.right

                    height: isMobile ? 40 : 48
                    z: 2
                    visible: true
                    color: Theme.colorForeground

                    RowLayout {
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.componentMargin
                        anchors.right: parent.right
                        anchors.rightMargin: Theme.componentMargin

                        height: parent.height
                        spacing: 12

                        ButtonClear {
                            text: qsTr("Connect")
                            onClicked: currentDevice.actionConnect(true)
                        }
                    }
                }

                ////////////////

                Column {
                    id: contentArea
                    anchors.top: infoArea.bottom
                    anchors.topMargin: 16
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.right: parent.right
                    anchors.rightMargin: 16 + (singleColumn ? 0 : parent.width * 0.5)
                    spacing: 16

                    opacity: currentDevice.connected ? 1 : 0.33
                    enabled: currentDevice.connected

                    ////////

                    Column {
                        spacing: 4
                        opacity: 0.66

                        Text {
                            text: "battery: " + currentDevice.deviceBattery
                        }
                        Text {
                            text: "firmware: " + currentDevice.deviceFirmware
                        }
                        Text {
                            text: "calibrated: " + currentDevice.calibrated
                        }
                        Text {
                            text: "moving: " + currentDevice.moving
                        }
                        Text {
                            text: "direction: " + currentDevice.direction
                        }
                        Text {
                            text: "position: " + currentDevice.position
                        }
                        Text {
                            text: "open: " + currentDevice.open
                        }
                        Text {
                            text: "lightlevel: " + currentDevice.lightlevel
                        }
                    }

                    ////////
                }

                ////////////////
            }
        }

        ////////////////////////////////////////////////////////////////////////
    }
}
