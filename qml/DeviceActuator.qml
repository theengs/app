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
        if (!clickedDevice.isActuator) return

        // set device
        if (currentDevice !== clickedDevice) currentDevice = clickedDevice

        // connect device
        currentDevice.actionConnect()

        // load screen
        if (!deviceActuator.active) deviceActuator.active = true
        deviceActuator.item.loadDevice()

        // change screen
        appContent.state = "DeviceActuator"
    }

    ////////

    function backAction() {
        if (deviceActuator.status === Loader.Ready)
            deviceActuator.item.backAction()
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
            if (!currentDevice.isActuator) return
            //console.log("deviceActuator // updateHeader() >> " + currentDevice)

            // Status
            updateStatusText()
        }

        function updateData() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isActuator) return
            //console.log("deviceActuator // updateData() >> " + currentDevice)
        }

        function updateStatusText() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isActuator) return
            //console.log("deviceActuator // updateStatusText() >> " + currentDevice)

            // Status
            textStatus.text = UtilsDeviceSensors.getDeviceStatusText(currentDevice.status)

            if (currentDevice.status === DeviceUtils.DEVICE_OFFLINE &&
                (currentDevice.isDataFresh_rt() || currentDevice.isDataToday())) {
                if (currentDevice.lastUpdateMin <= 1)
                    textStatus.text = qsTr("Synced")
                else
                    textStatus.text = qsTr("Synced %1 ago").arg(currentDevice.lastUpdateStr)
            }
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

                Rectangle { // rectangle indicator // mimic switchbot s1
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.height / 3)

                    width: singleColumn ? actuatorBox.height * 0.75 : actuatorBox.width * 0.66
                    height: width*0.75
                    radius: 32
                    color: Qt.alpha(cccc, 0.1)
                    border.width: 2
                    border.color: Qt.alpha(cccc, 0.33)

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
                                if (currentDevice.deviceModel === "X1") {
                                    if (currentDevice.mode === "on/off") return qsTr("Switch mode")
                                    else if (currentDevice.mode === "onestate") return qsTr("Press mode")
                                    else return qsTr("Unknown mode")
                                }
                                if (currentDevice.mode === "on/off") return qsTr("on/off")
                                else if (currentDevice.mode === "onestate") return qsTr("one state")
                                return currentDevice.mode
                            }

                            font.pixelSize: isPhone ? 22 : 26
                            font.bold: false
                            color: cccc
                            opacity: 0.66
                        }

                        Text { // value
                            anchors.horizontalCenter: parent.horizontalCenter

                            text: {
                                if (currentDevice.mode === "on/off") return currentDevice.state ? "on" : "off"
                                else if (currentDevice.mode === "onestate") return currentDevice.state
                                else return "?"
                            }

                            font.pixelSize: isPhone ? 26 : 30
                            font.bold: true
                            color: cccc
                            opacity: 1
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

                ////////////////
            }

            ////////////////
        }

        ////////////////////////////////////////////////////////////////////////
    }
}
