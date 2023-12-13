import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import ThemeEngine

import DeviceUtils
import "qrc:/js/UtilsDeviceSensors.js" as UtilsDeviceSensors

import PresetUtils
import "qrc:/js/UtilsPresets.js" as UtilsPresets

Loader {
    id: deviceProbe

    property var currentDevice: null
    property var currentPreset: null
    property int currentInterval: 0

    ////////

    function loadDevice(clickedDevice) {
        if (typeof clickedDevice === "undefined" || !clickedDevice) return
        if (!clickedDevice.isProbe) return

        // set device
        if (currentDevice !== clickedDevice) currentDevice = clickedDevice

        // load screen
        if (!deviceProbe.active) {
            deviceProbe.active = true
        }
        deviceProbe.item.loadDevice()

        // change screen
        appContent.state = "DeviceProbe"
        if (isMobile) mobileUI.setScreenAlwaysOn(true)
    }

    ////////

    function backAction() {
        if (deviceProbe.status === Loader.Ready)
            deviceProbe.item.backAction()
    }

    ////////////////////////////////////////////////////////////////////////////

    active: false
    asynchronous: false

    sourceComponent: Item {
        id: itemDeviceProbe
        implicitWidth: 480
        implicitHeight: 720

        focus: parent.focus

        property alias probeChart: graphLoader.item

        property string cccc: headerUnicolor ? Qt.lighter(Theme.colorHeaderContent, 1.33) : "white"

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
            //console.log("DeviceProbe // loadDevice() >> " + currentDevice)

            graphLoader.source = "" // force graph reload
            loadGraph()

            if (currentDevice.hasProbesBBQ) currentDevice.startRtCapture(true)

            currentPreset = presetsManager.getPreset(currentDevice.preset)
            currentInterval = currentDevice.realtimeWindow
            if (currentInterval === 60) selectorInterval.currentSelection = 3
            else if (currentInterval === 30) selectorInterval.currentSelection = 2
            else if (currentInterval === 10) selectorInterval.currentSelection = 1
            else selectorInterval.currentSelection = 0

            updateHeader()
            updateData()
        }

        function updateHeader() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isProbe) return
            //console.log("DeviceProbe // updateHeader() >> " + currentDevice)

            // Status
            updateStatusText()
        }

        function updateData() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isProbe) return
            //console.log("DeviceProbe // updateData() >> " + currentDevice)
        }

        function updateStatusText() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isProbe) return
            //console.log("DeviceProbe // updateStatusText() >> " + currentDevice)

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
            if (currentDevice.hasProbesTPMS) return

            if (graphLoader.status !== Loader.Ready) {
                graphLoader.source = "ChartProbeRealTime.qml"
                //graphLoader.source = "ChartProbeDataAio.qml" // disabled
            }

            if (graphLoader.status === Loader.Ready) {
                probeChart.loadGraph()
                probeChart.updateGraph()
            }
        }
        function updateGraph() {
            if (currentDevice.hasProbesTPMS) return

            if (graphLoader.status === Loader.Ready) probeChart.updateGraph()
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
            if (isMobile) mobileUI.setScreenAlwaysOn(false)
        }
        function isHistoryMode() {
            if (graphLoader.status === Loader.Ready) return probeChart.isIndicator()
            return false
        }
        function resetHistoryMode() {
            if (graphLoader.status === Loader.Ready) probeChart.resetIndicator()
        }

        ////////////////////////////////////////////////////////////////////////

        Flow {
            anchors.fill: parent

            Rectangle {
                id: probeBox

                property int dimboxw: Math.min(deviceProbe.width * 0.4, isPhone ? 320 : 600)
                property int dimboxh: Math.max(deviceProbe.height * (columnTPMS.visible ? 0.5 : 0.333), isPhone ? 200 : 256)

                width: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return parent.width
                    return singleColumn ? parent.width : dimboxw
                }
                height: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return dimboxh
                    if (isMobile && singleColumn && currentDevice.hasProbesBBQ) return columnBBQ.height + 48 + 24
                    return singleColumn ? dimboxh : parent.height
                }

                color: Theme.colorHeader
                z: 5

                MouseArea { anchors.fill: parent } // prevent clicks below this area




                IconSvg { // sensor disconnected ///////////////////////////////
                    width: isMobile ? 96 : 128
                    height: isMobile ? 96 : 128
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.headerHeight / 2)

                    visible: !currentDevice.hasData
                    source: "qrc:/assets/icons_material/baseline-bluetooth_disabled-24px.svg"
                    color: cccc
                }




                Column { // column BBQ probe ///////////////////////////////////
                    id: columnBBQ

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.headerHeight * 0.42)
                    spacing: 24

                    visible: (currentDevice.hasData && currentDevice.hasProbesBBQ)

                    property int psw: ((probeBox.width - 3*pss) / 2)
                    property int psh: isPhone ? 44 : 48
                    property int pss: isPhone ? 24 : 40
                    property int fz1: isPhone ? 28 : 32
                    property int fz2: isPhone ? 18 : 20

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 12

                        visible: isDesktop

                        Text { // deviceName
                            anchors.verticalCenter: parent.verticalCenter

                            text: currentDevice.deviceName
                            font.pixelSize: columnBBQ.fz1
                            font.bold: false
                            color: cccc
                            opacity: 0.66
                        }

                        IconSvg { //imageBattery
                            width: isPhone ? 20 : 24
                            height: isPhone ? 32 : 36
                            rotation: 90
                            anchors.verticalCenter: parent.verticalCenter

                            visible: (currentDevice.hasBattery && currentDevice.deviceBattery >= 0)
                            source: UtilsDeviceSensors.getDeviceBatteryIcon(currentDevice.deviceBattery)
                            fillMode: Image.PreserveAspectCrop
                            color: cccc
                        }
                    }

                    ////

                    Row {
                        spacing: columnBBQ.pss

                        Rectangle { // #1
                            width: columnBBQ.psw
                            height: columnBBQ.psh
                            radius: 8
                            clip: true
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)

                            Rectangle {
                                anchors.top: parent.top
                                anchors.topMargin: 2
                                anchors.left: parent.left
                                anchors.leftMargin: 2
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: 2

                                width: columnBBQ.psh
                                radius: 6
                                opacity: 0.60
                                color: Theme.colorMaterialBlue
                            }

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                text: "#1"
                                opacity: 0.66
                                font.pixelSize: columnBBQ.fz2
                                font.bold: false
                                color: cccc
                            }

                            Text {
                                anchors.right: parent.right
                                anchors.rightMargin: isPhone ? 8 : 12
                                anchors.verticalCenter: parent.verticalCenter

                                visible: (currentDevice.temperature1 < -80)
                                text: qsTr("unplugged")
                                opacity: 0.66
                                font.pixelSize: columnBBQ.fz2
                                font.bold: false
                                color: cccc
                            }

                            Row {
                                anchors.right: parent.right
                                anchors.rightMargin: isPhone ? 8 : 12
                                anchors.verticalCenter: parent.verticalCenter
                                visible: (currentDevice.temperature1 > -80)

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter

                                    visible: (currentDevice.temperature1 > -80)
                                    text: currentDevice.temperature1.toFixed(0)
                                    font.pixelSize: columnBBQ.fz1
                                    font.bold: false
                                    color: cccc
                                }
                                Text {
                                    anchors.top: parent.top
                                    anchors.topMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                    opacity: 0.66
                                    font.pixelSize: columnBBQ.fz2
                                    font.bold: false
                                    color: cccc
                                }
                            }
                        }

                        Rectangle { // #2
                            width: columnBBQ.psw
                            height: columnBBQ.psh
                            radius: 8
                            clip: true
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)

                            Rectangle {
                                anchors.top: parent.top
                                anchors.topMargin: 2
                                anchors.left: parent.left
                                anchors.leftMargin: 2
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: 2

                                width: columnBBQ.psh
                                radius: 6
                                opacity: 0.60
                                color: Theme.colorMaterialDeepPurple
                            }

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                text: "#2"
                                opacity: 0.66
                                font.pixelSize: columnBBQ.fz2
                                font.bold: false
                                color: cccc
                            }

                            Text {
                                anchors.right: parent.right
                                anchors.rightMargin: isPhone ? 8 : 12
                                anchors.verticalCenter: parent.verticalCenter

                                visible: (currentDevice.temperature2 < -80)
                                text: qsTr("unplugged")
                                opacity: 0.66
                                font.pixelSize: columnBBQ.fz2
                                font.bold: false
                                color: cccc
                            }

                            Row {
                                anchors.right: parent.right
                                anchors.rightMargin: isPhone ? 8 : 12
                                anchors.verticalCenter: parent.verticalCenter
                                visible: (currentDevice.temperature2 > -80)

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter

                                    visible: (currentDevice.temperature2 > -80)
                                    text: currentDevice.temperature2.toFixed(0)
                                    font.pixelSize: columnBBQ.fz1
                                    font.bold: false
                                    color: cccc
                                }
                                Text {
                                    anchors.top: parent.top
                                    anchors.topMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                    opacity: 0.66
                                    font.pixelSize: columnBBQ.fz2
                                    font.bold: false
                                    color: cccc
                                }
                            }
                        }
                    }

                    ////

                    Row {
                        spacing: columnBBQ.pss
                        visible: currentDevice.hasTemperature3

                        Rectangle { // #3
                            width: columnBBQ.psw
                            height: columnBBQ.psh
                            radius: 8
                            clip: true
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)

                            Rectangle {
                                anchors.top: parent.top
                                anchors.topMargin: 2
                                anchors.left: parent.left
                                anchors.leftMargin: 2
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: 2

                                width: columnBBQ.psh
                                radius: 6
                                opacity: 0.60
                                color: Theme.colorMaterialLightGreen
                            }

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                text: "#3"
                                opacity: 0.66
                                font.pixelSize: columnBBQ.fz2
                                font.bold: false
                                color: cccc
                            }

                            Text {
                                anchors.right: parent.right
                                anchors.rightMargin: isPhone ? 8 : 12
                                anchors.verticalCenter: parent.verticalCenter

                                visible: (currentDevice.temperature3 < -80)
                                text: qsTr("unplugged")
                                opacity: 0.66
                                font.pixelSize: columnBBQ.fz2
                                font.bold: false
                                color: cccc
                            }

                            Row {
                                anchors.right: parent.right
                                anchors.rightMargin: isPhone ? 8 : 12
                                anchors.verticalCenter: parent.verticalCenter
                                visible: (currentDevice.temperature3 > -80)

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter

                                    visible: (currentDevice.temperature3 > -80)
                                    text: currentDevice.temperature3.toFixed(0)
                                    font.pixelSize: columnBBQ.fz1
                                    font.bold: false
                                    color: cccc
                                }
                                Text {
                                    anchors.top: parent.top
                                    anchors.topMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                    opacity: 0.66
                                    font.pixelSize: columnBBQ.fz2
                                    font.bold: false
                                    color: cccc
                                }
                            }
                        }

                        Rectangle { // #4
                            width: columnBBQ.psw
                            height: columnBBQ.psh
                            radius: 8
                            clip: true
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)

                            Rectangle {
                                anchors.top: parent.top
                                anchors.topMargin: 2
                                anchors.left: parent.left
                                anchors.leftMargin: 2
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: 2

                                width: columnBBQ.psh
                                radius: 6
                                opacity: 0.60
                                color: Theme.colorYellow
                            }

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                text: "#4"
                                opacity: 0.66
                                font.pixelSize: columnBBQ.fz2
                                font.bold: false
                                color: cccc
                            }

                            Text {
                                anchors.right: parent.right
                                anchors.rightMargin: isPhone ? 8 : 12
                                anchors.verticalCenter: parent.verticalCenter

                                visible: (currentDevice.temperature4 < -80)
                                text: qsTr("unplugged")
                                opacity: 0.66
                                font.pixelSize: columnBBQ.fz2
                                font.bold: false
                                color: cccc
                            }

                            Row {
                                anchors.right: parent.right
                                anchors.rightMargin: isPhone ? 8 : 12
                                anchors.verticalCenter: parent.verticalCenter
                                visible: (currentDevice.temperature4 > -80)

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter

                                    visible: (currentDevice.temperature4 > -80)
                                    text: currentDevice.temperature4.toFixed(0)
                                    font.pixelSize: columnBBQ.fz1
                                    font.bold: false
                                    color: cccc
                                }
                                Text {
                                    anchors.top: parent.top
                                    anchors.topMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                    opacity: 0.66
                                    font.pixelSize: columnBBQ.fz2
                                    font.bold: false
                                    color: cccc
                                }
                            }
                        }
                    }

                    ////

                    Row {
                        spacing: columnBBQ.pss
                        visible: currentDevice.hasTemperature5

                        Rectangle {
                            width: columnBBQ.psw
                            height: columnBBQ.psh
                            radius: 8
                            clip: true
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)

                            Rectangle {
                                anchors.top: parent.top
                                anchors.topMargin: 2
                                anchors.left: parent.left
                                anchors.leftMargin: 2
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: 2

                                width: columnBBQ.psh
                                radius: 6
                                opacity: 0.60
                                color: Theme.colorOrange
                            }

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                text: "#5"
                                opacity: 0.66
                                font.pixelSize: columnBBQ.fz2
                                font.bold: false
                                color: cccc
                            }

                            Text {
                                anchors.right: parent.right
                                anchors.rightMargin: isPhone ? 8 : 12
                                anchors.verticalCenter: parent.verticalCenter

                                visible: (currentDevice.temperature5 < -80)
                                text: qsTr("unplugged")
                                opacity: 0.66
                                font.pixelSize: columnBBQ.fz2
                                font.bold: false
                                color: cccc
                            }

                            Row {
                                anchors.right: parent.right
                                anchors.rightMargin: isPhone ? 8 : 12
                                anchors.verticalCenter: parent.verticalCenter
                                visible: (currentDevice.temperature5 > -80)

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter

                                    text: currentDevice.temperature5.toFixed(0)
                                    font.pixelSize: columnBBQ.fz1
                                    font.bold: false
                                    color: cccc
                                }
                                Text {
                                    anchors.top: parent.top
                                    anchors.topMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                    opacity: 0.66
                                    font.pixelSize: columnBBQ.fz2
                                    font.bold: false
                                    color: cccc
                                }
                            }
                        }

                        Rectangle {
                            width: columnBBQ.psw
                            height: columnBBQ.psh
                            radius: 8
                            clip: true
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)

                            Rectangle {
                                anchors.top: parent.top
                                anchors.topMargin: 2
                                anchors.left: parent.left
                                anchors.leftMargin: 2
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: 2

                                width: columnBBQ.psh
                                radius: 6
                                opacity: 0.60
                                color: Theme.colorRed
                            }

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                text: "#6"
                                opacity: 0.66
                                font.pixelSize: columnBBQ.fz2
                                font.bold: false
                                color: cccc
                            }

                            Text {
                                anchors.right: parent.right
                                anchors.rightMargin: isPhone ? 8 : 12
                                anchors.verticalCenter: parent.verticalCenter

                                visible: (currentDevice.temperature6 < -80)
                                text: qsTr("unplugged")
                                opacity: 0.66
                                font.pixelSize: columnBBQ.fz2
                                font.bold: false
                                color: cccc
                            }

                            Row {
                                anchors.right: parent.right
                                anchors.rightMargin: isPhone ? 8 : 12
                                anchors.verticalCenter: parent.verticalCenter
                                visible: (currentDevice.temperature6 > -80)

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter

                                    visible: (currentDevice.temperature6 > -80)
                                    text: currentDevice.temperature6.toFixed(0)
                                    font.pixelSize: columnBBQ.fz1
                                    font.bold: false
                                    color: cccc
                                }
                                Text {
                                    anchors.top: parent.top
                                    anchors.topMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                    opacity: 0.66
                                    font.pixelSize: columnBBQ.fz2
                                    font.bold: false
                                    color: cccc
                                }
                            }
                        }

                        ////
                    }
                }




                Column { // column TPMS probe //////////////////////////////////
                    id: columnTPMS

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.headerHeight / 3)
                    spacing: columnTPMS.tsp

                    visible: (currentDevice.hasData && currentDevice.hasProbesTPMS)

                    property int tsp: singleColumn ? (isPhone ? 24 : 48) : 80
                    property int tsz: singleColumn ? (isPhone ? 120 : 140) : 180
                    property int fz1: isPhone ? 26 : 32
                    property int fz2: isPhone ? 22 : 28
                    property int fz3: isPhone ? 14 : 20

                    Row {
                        spacing: columnTPMS.tsp

                        Rectangle {
                            width: columnTPMS.tsz
                            height: columnTPMS.tsz
                            radius: columnTPMS.tsz

                            color: Qt.alpha(cccc, 0.1)
                            border.width: 8
                            border.color: currentDevice.alarm1 ? Theme.colorRed : Qt.alpha(cccc, 0.33)

                            IconSvg { // sensorDisconnected
                                width: 48; height: 48;
                                anchors.centerIn: parent
                                visible: (currentDevice.temperature1 <= -80)
                                source: "qrc:/assets/icons_material/baseline-bluetooth_disabled-24px.svg"
                                color: cccc
                            }

                            Column { // sensorData
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: -8
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.verticalCenterOffset: 8
                                visible: (currentDevice.temperature1 > -99)
                                spacing: isPhone ? -2 : 0

                                Text {
                                    text: (currentDevice.pressure1 / 1000).toFixed(1)
                                    font.bold: false
                                    font.pixelSize: columnTPMS.fz1
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.leftMargin: 2
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: "bar"
                                        opacity: 0.66
                                        font.pixelSize: columnTPMS.fz3
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                                Text {
                                    text: currentDevice.temperature1.toFixed(1)
                                    font.pixelSize: columnTPMS.fz2
                                    font.bold: false
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                        opacity: 0.66
                                        font.pixelSize: columnTPMS.fz3
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                                IconSvg {
                                    width: isPhone ? 20 : 24
                                    height: isPhone ? 32 : 36
                                    rotation: 90
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.horizontalCenterOffset: 8
                                    source: UtilsDeviceSensors.getDeviceBatteryIcon(currentDevice.battery1)
                                    fillMode: Image.PreserveAspectCrop
                                    color: cccc
                                }
                            }
                        }

                        Rectangle {
                            width: columnTPMS.tsz
                            height: columnTPMS.tsz
                            radius: columnTPMS.tsz

                            color: Qt.alpha(cccc, 0.1)
                            border.width: 8
                            border.color: currentDevice.alarm2 ? Theme.colorRed : Qt.alpha(cccc, 0.33)

                            IconSvg { // sensorDisconnected
                                width: 48; height: 48;
                                anchors.centerIn: parent
                                visible: (currentDevice.temperature2 <= -80)
                                source: "qrc:/assets/icons_material/baseline-bluetooth_disabled-24px.svg"
                                color: cccc
                            }

                            Column { // sensorData
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: -8
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.verticalCenterOffset: 8
                                visible: (currentDevice.temperature2 > -99)
                                spacing: isPhone ? -2 : 0

                                Text {
                                    text: (currentDevice.pressure2 / 1000).toFixed(1)
                                    font.bold: false
                                    font.pixelSize: columnTPMS.fz1
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.leftMargin: 2
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: "bar"
                                        opacity: 0.66
                                        font.pixelSize: columnTPMS.fz3
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                                Text {
                                    text: currentDevice.temperature2.toFixed(1)
                                    font.pixelSize: columnTPMS.fz2
                                    font.bold: false
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                        opacity: 0.66
                                        font.pixelSize: columnTPMS.fz3
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                                IconSvg {
                                    width: isPhone ? 20 : 24
                                    height: isPhone ? 32 : 36
                                    rotation: 90
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.horizontalCenterOffset: 8
                                    source: UtilsDeviceSensors.getDeviceBatteryIcon(currentDevice.battery2)
                                    fillMode: Image.PreserveAspectCrop
                                    color: cccc
                                }
                            }
                        }
                    }

                    Row {
                        spacing: columnTPMS.tsp

                        Rectangle {
                            width: columnTPMS.tsz
                            height: columnTPMS.tsz
                            radius: columnTPMS.tsz

                            color: Qt.alpha(cccc, 0.1)
                            border.width: 8
                            border.color: currentDevice.alarm3 ? Theme.colorRed : Qt.alpha(cccc, 0.33)

                            IconSvg { // sensorDisconnected
                                width: 48; height: 48;
                                anchors.centerIn: parent
                                visible: (currentDevice.temperature3 <= -80)
                                source: "qrc:/assets/icons_material/baseline-bluetooth_disabled-24px.svg"
                                color: cccc
                            }

                            Column { // sensorData
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: -8
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.verticalCenterOffset: 8
                                visible: (currentDevice.temperature3 > -99)
                                spacing: isPhone ? -2 : 0

                                Text {
                                    text: (currentDevice.pressure3 / 1000).toFixed(1)
                                    font.bold: false
                                    font.pixelSize: columnTPMS.fz1
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.leftMargin: 2
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: "bar"
                                        opacity: 0.66
                                        font.pixelSize: columnTPMS.fz3
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                                Text {
                                    text: currentDevice.temperature3.toFixed(1)
                                    font.pixelSize: columnTPMS.fz2
                                    font.bold: false
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                        opacity: 0.66
                                        font.pixelSize: columnTPMS.fz3
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                                IconSvg {
                                    width: isPhone ? 20 : 24
                                    height: isPhone ? 32 : 36
                                    rotation: 90
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.horizontalCenterOffset: 8
                                    source: UtilsDeviceSensors.getDeviceBatteryIcon(currentDevice.battery3)
                                    fillMode: Image.PreserveAspectCrop
                                    color: cccc
                                }
                            }
                        }

                        Rectangle {
                            width: columnTPMS.tsz
                            height: columnTPMS.tsz
                            radius: columnTPMS.tsz

                            color: Qt.alpha(cccc, 0.1)
                            border.width: 8
                            border.color: currentDevice.alarm4 ? Theme.colorRed : Qt.alpha(cccc, 0.33)

                            IconSvg { // sensorDisconnected
                                width: 48; height: 48;
                                anchors.centerIn: parent
                                visible: (currentDevice.temperature4 <= -80)
                                source: "qrc:/assets/icons_material/baseline-bluetooth_disabled-24px.svg"
                                color: cccc
                            }

                            Column { // sensorData
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: -8
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.verticalCenterOffset: 8
                                visible: (currentDevice.temperature4 > -99)
                                spacing: isPhone ? -2 : 0

                                Text {
                                    text: (currentDevice.pressure4 / 1000).toFixed(1)
                                    font.bold: false
                                    font.pixelSize: columnTPMS.fz1
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.leftMargin: 2
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: "bar"
                                        opacity: 0.66
                                        font.pixelSize: columnTPMS.fz3
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                                Text {
                                    text: currentDevice.temperature4.toFixed(1)
                                    font.pixelSize: columnTPMS.fz2
                                    font.bold: false
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                        opacity: 0.66
                                        font.pixelSize: columnTPMS.fz3
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                                IconSvg {
                                    width: isPhone ? 20 : 24
                                    height: isPhone ? 32 : 36
                                    rotation: 90
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.horizontalCenterOffset: 8
                                    source: UtilsDeviceSensors.getDeviceBatteryIcon(currentDevice.battery4)
                                    fillMode: Image.PreserveAspectCrop
                                    color: cccc
                                }
                            }
                        }
                    }
                }

                Text { // deviceName TPMS
                    anchors.centerIn: columnTPMS
                    visible: (columnTPMS.visible && isDesktop)

                    text: currentDevice.deviceName
                    font.pixelSize: columnTPMS.fz2
                    font.bold: false
                    color: cccc
                    opacity: 0.66
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

                        source: "qrc:/assets/icons_material/duotone-schedule-24px.svg"
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

                        source: "qrc:/assets/icons_material/duotone-edit-24px.svg"
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

                        source: "qrc:/assets/icons_material/duotone-pin_drop-24px.svg"
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

            ////////////////

            Item {
                width: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return parent.width
                    return singleColumn ? parent.width : (parent.width - probeBox.width)
                }
                height: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return (parent.height - probeBox.height)
                    return singleColumn ? (parent.height - probeBox.height) : parent.height
                }

                ////////

                Column {
                    id: presetStuff
                    anchors.left: parent.left
                    anchors.right: parent.right

                    visible: currentDevice.hasProbesBBQ
                    height: {
                        if (currentDevice.hasProbesBBQ && currentPreset) return 40 + 36
                        if (currentDevice.hasProbesBBQ) return 40
                        return 0
                    }
                    z: 2

                    ////////

                    Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 40

                        color: Theme.colorForeground

                        RowLayout {
                            anchors.left: parent.left
                            anchors.leftMargin: Theme.componentMargin
                            anchors.right: parent.right
                            anchors.rightMargin: Theme.componentMargin
                            height: parent.height
                            spacing: 12

                            Rectangle {
                                Layout.preferredHeight: parent.height
                                Layout.preferredWidth: legendPreset.contentWidth + 12

                                visible: !singleColumn
                                color: Qt.darker(Theme.colorForeground, 1.03)

                                Text {
                                    id: legendPreset
                                    anchors.centerIn: parent
                                    text: qsTr("PRESET")
                                    textFormat: Text.PlainText
                                    color: Theme.colorText
                                }
                            }

                            SelectorMenuItem {
                                Layout.preferredHeight: 32
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                                highlighted: true

                                text: {
                                    if (currentPreset) return currentPreset.name
                                    return UtilsPresets.getPresetType(0)
                                }
                                source: {
                                    if (currentPreset) return UtilsPresets.getPresetIcon(currentPreset.type)
                                    return UtilsPresets.getPresetIcon(0)
                                }
                                sourceSize: 20

                                PopupPresetSelection {
                                    id: popupPresetSelection
                                    onSelected: (name) => {
                                        currentDevice.preset = name
                                        currentPreset = presetsManager.getPreset(currentDevice.preset)
                                    }
                                }

                                onClicked: {
                                    popupPresetSelection.open()
                                }
                            }

                            Item {
                                Layout.fillWidth: singleColumn
                                Layout.preferredHeight: 32
                            }

                            Rectangle {
                                Layout.preferredWidth: legendPreset.contentWidth + 12
                                Layout.preferredHeight: parent.height

                                visible: !singleColumn
                                color: Qt.darker(Theme.colorForeground, 1.03)

                                Text {
                                    id: legendInterval
                                    anchors.centerIn: parent
                                    text: qsTr("INTERVAL")
                                    textFormat: Text.PlainText
                                    color: Theme.colorText
                                }
                            }

                            SelectorMenu {
                                id: selectorInterval
                                Layout.preferredWidth: width
                                Layout.preferredHeight: 32

                                model: ListModel {
                                    id: intervalModel
                                    ListElement { idx: 0; txt: qsTr("5m"); itv: 5; src: ""; sz: 16; }
                                    ListElement { idx: 1; txt: qsTr("10m"); itv: 10; src: ""; sz: 16; }
                                    ListElement { idx: 2; txt: qsTr("30m"); itv: 30; src: ""; sz: 16; }
                                    ListElement { idx: 3; txt: qsTr("60m"); itv: 60; src: ""; sz: 16; }
                                }

                                currentSelection: {
                                    if (currentInterval === 60) return 3
                                    if (currentInterval === 30) return 2
                                    if (currentInterval === 10) return 1
                                    return 0
                                }
                                onMenuSelected: (index) => {
                                    currentSelection = index
                                    var value = model.get(index).itv

                                    currentInterval = value
                                    if (currentDevice) currentDevice.realtimeWindow = value

                                    probeChart.updateGraph()
                                }
                            }
                        }
                    }

                    ////////

                    Flickable {
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.componentMargin
                        anchors.right: parent.right
                        anchors.rightMargin: Theme.componentMargin
                        height: 36
                        visible: currentPreset

                        contentWidth: presetLegendRow.width
                        contentHeight: 36

                        flickableDirection: Flickable.HorizontalFlick
                        boundsBehavior: Flickable.StopAtBounds

                        Row {
                            id: presetLegendRow
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 12

                            Repeater {
                                model: currentPreset && currentPreset.ranges

                                Item {
                                    height: 24
                                    width: txt.contentWidth + 12

                                    Rectangle {
                                        anchors.fill: parent
                                        radius: 4
                                        color: Theme.colorRed
                                        opacity: 0.66 + ((index) * 0.05)
                                    }

                                    Text {
                                        id: txt
                                        anchors.centerIn: parent
                                        text: modelData.name
                                        color: "white"
                                    }
                                }
                            }
                        }
                    }

                    ////////
                }

                ////////

                Loader {
                    id: graphLoader
                    anchors.top: parent.top
                    anchors.topMargin: presetStuff.height
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    asynchronous: true
                    onLoaded: {
                        //console.log("graphLoader::onLoaded()")
                        probeChart.loadGraph()
                        probeChart.updateGraph()
                    }
                }

                ////////
            }

            ////////////////
        }
    }
}
