import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import ComponentLibrary
import DeviceUtils
import DeviceUtilsTheengs
import DeviceUtilsSwitchBot
import "qrc:/js/UtilsDeviceSensors.js" as UtilsDeviceSensors
import "qrc:/ComponentLibrary/UtilsNumber.js" as UtilsNumber

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
        }

        Connections {
            target: settingsManager
            function onAppLanguageChanged() {
                updateStatusText()
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
            //console.log("deviceActuatorWindow // loadDevice() >> " + currentDevice)

            loadGraph()
            updateHeader()
            updateData()
        }

        function updateHeader() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isActuatorWindow) return
            //console.log("deviceActuatorWindow // updateHeader() >> " + currentDevice)

            updateStatusText()
        }

        function updateData() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isActuatorWindow) return
            //console.log("deviceActuatorWindow // updateData() >> " + currentDevice)
        }

        function updateStatusText() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isActuatorWindow) return
            //console.log("deviceActuatorWindow // updateStatusText() >> " + currentDevice)

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

                //MouseArea { anchors.fill: parent } // prevent clicks below this area

                ////////

                Item { // square indicator (mimic a window)
                    width: singleColumn ? actuatorBox.height * 0.85 : actuatorBox.width * 0.75
                    height: width

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.height * 0.33)

                    ////

                    Rectangle {
                        id: indicatorBackground
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
                            width: {
                                if ((currentDevice.deviceModel === "W270160X")) {
                                    return ((indicatorBackground.width-4) * ((100-currentDevice.open)/100.0))
                                } else {
                                    return ((indicatorBackground.width-4) * (currentDevice.position/100.0))
                                }
                            }
                            color: Theme.colorPrimary
                        }
                    }

                    ////

                    IconSvg { // sensorDisconnected
                        width: isMobile ? 96 : 128
                        height: isMobile ? 96 : 128
                        anchors.centerIn: parent

                        visible: !(currentDevice.available || currentDevice.connected) // !currentDevice.hasData
                        source: "qrc:/IconLibrary/material-symbols/bluetooth_disabled.svg"
                        color: cccc
                    }

                    ////

                    Column {
                        anchors.centerIn: parent

                        visible: (currentDevice.available || currentDevice.connected) // currentDevice.hasData
                        spacing: 0
/*
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
*/
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

                        Row {
                            anchors.horizontalCenter: parent.horizontalCenter

                            IconSvg {
                                width: 30
                                height: 32

                                visible: (currentDevice.hasBattery && currentDevice.deviceBattery >= 0)
                                source: UtilsDeviceSensors.getDeviceBatteryIcon(currentDevice.deviceBattery)
                                color: cccc
                                rotation: 90
                                fillMode: Image.PreserveAspectCrop
                            }
                            IconSvg {
                                width: 24
                                height: 24

                                visible: (currentDevice.solar)
                                source: "qrc:/assets/icons_material/solar_power.svg"
                                color: cccc
                                fillMode: Image.PreserveAspectCrop
                            }
                        }
                    }

                    ////
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

                Rectangle {
                    id: infoArea
                    anchors.left: parent.left
                    anchors.right: parent.right
                    z: 2

                    visible: false
                    height: visible ? (isMobile ? 40 : 48) : 0
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
                    anchors.rightMargin: 16 + (!singleColumn ? parent.width * 0.33 : 0)

                    //opacity: currentDevice.connected ? 1 : 0.33
                    enabled: currentDevice.connected
                    spacing: 20

                    ////////

                    Column {
                        anchors.left: parent.left
                        anchors.right: parent.right

                        visible: (currentDevice.deviceModel === "W070160X") // Curtain 2/3
                        spacing: 20

                        ////

                        SliderValueSolid {
                            id: actionSlider_curtain
                            anchors.left: parent.left
                            anchors.right: parent.right
                            hhh: 40

                            from: 0
                            to: 100
                            value: currentDevice.position

                            onMoved: {
                                currentDevice.actionMove(value)
                                actionSlider_curtain.value = value
                            }
                        }

                        ////

                        Row {
                            id: actionRow_curtain
                            anchors.horizontalCenter: parent.horizontalCenter
                            spacing: 64

                            property int www: Math.min(112, (parent.width - 16*2 - 64*2) / 3)

                            Column {
                                width: actionRow_curtain.www
                                spacing: 12

                                RoundButtonSolid {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: actionRow_curtain.www; height: actionRow_curtain.www;

                                    source: "qrc:/assets/icons_material/curtains_closed.svg"
                                    onClicked: {
                                        if (currentDevice.actionAction(DeviceUtilsSwitchBot.ACTION_OPEN)) {
                                            actionSlider_curtain.value = 0
                                        }
                                    }
                                }
                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: qsTr("Open")
                                    textFormat: Text.PlainText
                                    color: Theme.colorText
                                }
                            }

                            Column {
                                width: actionRow_curtain.www
                                spacing: 12

                                RoundButtonSolid {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: actionRow_curtain.www; height: actionRow_curtain.www;

                                    source: "qrc:/IconLibrary/material-symbols/media/pause-fill.svg"
                                    onClicked: {
                                        if (currentDevice.actionAction(DeviceUtilsSwitchBot.ACTION_STOP)) {
                                            //actionSlider_curtain.value = currentDevice.position
                                        }
                                    }
                                }
                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: qsTr("Pause")
                                    textFormat: Text.PlainText
                                    color: Theme.colorText
                                }
                            }

                            Column {
                                width: actionRow_curtain.www
                                spacing: 12

                                RoundButtonSolid {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: actionRow_curtain.www; height: actionRow_curtain.www;

                                    source: "qrc:/assets/icons_material/curtains_closed-fill.svg"
                                    onClicked: {
                                        onClicked: {
                                            if (currentDevice.actionAction(DeviceUtilsSwitchBot.ACTION_CLOSE)) {
                                                actionSlider_curtain.value = 100
                                            }
                                        }
                                    }
                                }
                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: qsTr("Close")
                                    textFormat: Text.PlainText
                                    color: Theme.colorText
                                }
                            }
                        }

                        ////
                    }

                    ////////

                    RowLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right

                        visible: (currentDevice.deviceModel === "W270160X") // Blind Tilt
                        spacing: 16

                        ////

                        Column {
                            id: actionRow_blind
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter
                            spacing: 32

                            property int www: Math.min(80, (parent.height - 16*2 - 32*2) / 3)

                            Row {
                                height: actionRow_blind.www
                                spacing: 12

                                RoundButtonSolid {
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: actionRow_blind.www; height: actionRow_blind.www;

                                    source: "qrc:/assets/icons_material/window_closed-fill.svg"
                                    onClicked: {
                                        if (currentDevice.actionAction(DeviceUtilsSwitchBot.ACTION_CLOSE_UP)) {
                                            actionMiddleSlider_blind.value = 100
                                        }
                                    }
                                }
                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: qsTr("Close Up")
                                    textFormat: Text.PlainText
                                    color: Theme.colorText
                                }
                            }

                            Row {
                                height: actionRow_blind.www
                                spacing: 12

                                RoundButtonSolid {
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: actionRow_blind.www; height: actionRow_blind.www;

                                    source: "qrc:/assets/icons_material/window_closed.svg"
                                    onClicked: {
                                        if (currentDevice.actionAction(DeviceUtilsSwitchBot.ACTION_OPEN)) {
                                            actionMiddleSlider_blind.value = 0
                                        }
                                    }
                                }
                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: qsTr("Fully open")
                                    textFormat: Text.PlainText
                                    color: Theme.colorText
                                }
                            }

                            Row {
                                height: actionRow_blind.www
                                spacing: 12

                                RoundButtonSolid {
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: actionRow_blind.www; height: actionRow_blind.www;

                                    source: "qrc:/assets/icons_material/window_closed-fill.svg"
                                    onClicked: {
                                        if (currentDevice.actionAction(DeviceUtilsSwitchBot.ACTION_CLOSE_DOWN)) {
                                            actionMiddleSlider_blind.value = -100
                                        }
                                    }
                                }
                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: qsTr("Close Down")
                                    textFormat: Text.PlainText
                                    color: Theme.colorText
                                }
                            }
                        }

                        ////

                        MiddleSliderValueSolid {
                            id: actionMiddleSlider_blind
                            orientation: Qt.Vertical
                            Layout.preferredHeight: 320
                            hhh: 40

                            from: -100
                            to: 100
                            value: currentDevice.open_blindtilt

                            onMoved: {
                                var letsgo = UtilsNumber.mapNumber(value, -100, 100, 0, 100)
                                currentDevice.actionMove(letsgo)
                            }
                        }

                        ////
                    }

                    ////////

                    Loader {
                        active: utilsApp.isDebugBuild()
                        asynchronous: true
                        sourceComponent: Column {
                            spacing: 4

                            Text {
                                text: "battery: " + currentDevice.deviceBattery
                                color: Theme.colorSubText
                            }
                            Text {
                                text: "firmware: " + currentDevice.deviceFirmware
                                color: Theme.colorSubText
                            }
                            Text {
                                text: "calibrated: " + currentDevice.calibrated
                                color: Theme.colorSubText
                            }
                            Text {
                                text: "moving: " + currentDevice.moving
                                color: Theme.colorSubText
                            }
                            Text {
                                text: "direction: " + currentDevice.direction
                                color: Theme.colorSubText
                            }
                            Text {
                                text: "position: " + currentDevice.position
                                color: Theme.colorSubText
                            }
                            Text {
                                text: "open: " + currentDevice.open
                                color: Theme.colorSubText
                            }
                            Text {
                                text: "solar panel: " + currentDevice.solar
                                color: Theme.colorSubText
                            }
                            Text {
                                text: "lightlevel: " + currentDevice.lightlevel
                                color: Theme.colorSubText
                            }
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
