import QtQuick 2.15
import QtQuick.Controls 2.15

import ThemeEngine 1.0
import DeviceUtils 1.0
import "qrc:/js/UtilsDeviceSensors.js" as UtilsDeviceSensors

Loader {
    id: deviceScale

    property var currentDevice: null

    ////////

    function loadDevice(clickedDevice) {
        // set device
        if (typeof clickedDevice === "undefined" || !clickedDevice) return
        if (!clickedDevice.isScale) return
        if (clickedDevice === currentDevice) return
        currentDevice = clickedDevice

        // load screen
        deviceScale.active = true
        deviceScale.item.loadDevice()
    }

    ////////

    function backAction() {
        if (deviceScale.status === Loader.Ready)
            deviceScale.item.backAction()
    }

    ////////////////////////////////////////////////////////////////////////////

    active: false

    asynchronous: false
    sourceComponent: Item {
        id: itemDeviceScale
        implicitWidth: 480
        implicitHeight: 720

        focus: parent.focus

        property alias weightChart: graphLoader.item

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
            //console.log("DeviceScale // loadDevice() >> " + currentDevice)

            graphLoader.source = "" // force graph reload

            loadGraph()
            updateHeader()
            updateData()
        }

        function updateHeader() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isScale) return
            //console.log("DeviceScale // updateHeader() >> " + currentDevice)

            // Status
            updateStatusText()
        }

        function updateData() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isScale) return
            //console.log("DeviceScale // updateData() >> " + currentDevice)
        }

        function updateStatusText() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isScale) return
            //console.log("DeviceScale // updateStatusText() >> " + currentDevice)

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
            if (graphLoader.status !== Loader.Ready) {
                graphLoader.source = "ChartScaleDataAio.qml"
            }

            if (graphLoader.status === Loader.Ready) {
                weightChart.loadGraph()
                weightChart.updateGraph()
            }
        }
        function updateGraph() {
            if (graphLoader.status === Loader.Ready) weightChart.updateGraph()
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
            if (graphLoader.status === Loader.Ready) return weightChart.isIndicator()
            return false
        }
        function resetHistoryMode() {
            if (graphLoader.status === Loader.Ready) weightChart.resetIndicator()
        }

        ////////////////////////////////////////////////////////////////////////////

        Flow {
            anchors.fill: parent

            Rectangle {
                id: scaleBox

                property int dimboxw: Math.min(deviceScale.width * 0.4, isPhone ? 300 : 600)
                property int dimboxh: Math.max(deviceScale.height * 0.333, isPhone ? 180 : 256)

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

                IconSvg { // sensorDisconnected
                    width: isMobile ? 96 : 128
                    height: isMobile ? 96 : 128
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.height / 3)

                    visible: !currentDevice.hasData
                    source: "qrc:/assets/icons_material/baseline-bluetooth_disabled-24px.svg"
                    color: cccc
                }

                Column {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.height / 3)
                    spacing: 0

                    Rectangle { // scale
                        width: singleColumn ? scaleBox.height * 0.75 : scaleBox.width * 0.66
                        height: width
                        radius: 16
                        color: Qt.alpha(cccc, 0.1)
                        border.width: 2
                        border.color: Qt.alpha(cccc, 0.33)

                        Rectangle {
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.margins: 12
                            width: 28; height: 28; radius: 28;
                            color: Qt.alpha(cccc, 0.33)
                        }
                        Rectangle {
                            anchors.top: parent.top
                            anchors.right: parent.right
                            anchors.margins: 12
                            width: 28; height: 28; radius: 28;
                            color: Qt.alpha(cccc, 0.33)
                        }
                        Rectangle {
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.margins: 12
                            width: 28; height: 28; radius: 28;
                            color: Qt.alpha(cccc, 0.33)
                        }
                        Rectangle {
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            anchors.margins: 12
                            width: 28; height: 28; radius: 28;
                            color: Qt.alpha(cccc, 0.33)
                        }

                        Rectangle {
                            anchors.top: parent.top
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.margins: 12
                            width: 64; height: 28; radius: 12;
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)
                        }

                        Column {
                            id: stuff
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.verticalCenterOffset: 4

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: (settingsManager.tempUnit === 'C') ? currentDevice.weight.toFixed(1) : (currentDevice.weight * 2.20462).toFixed(1)
                                font.pixelSize: isPhone ? 30 : 32
                                color: cccc

                                Text {
                                    anchors.left: parent.right
                                    anchors.leftMargin: 2
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "kg" : "lb"
                                    opacity: 0.66
                                    font.pixelSize: isPhone ? 20 : 22
                                    font.bold: false
                                    color: cccc
                                }
                            }

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: currentDevice.impedance
                                color: cccc
                                font.pixelSize: isPhone ? 26 : 28

                                visible: (currentDevice.hasImpedance && currentDevice.impedance > 0)

                                Text {
                                    anchors.left: parent.right
                                    anchors.leftMargin: 2
                                    anchors.verticalCenter: parent.verticalCenter

                                    text: "Ω"
                                    font.pixelSize: isPhone ? 20 : 22
                                    font.bold: false
                                    color: cccc
                                    opacity: 0.66
                                }
                            }
                        }

                        IconSvg {
                            id: imageBattery
                            width: isPhone ? 20 : 24
                            height: isPhone ? 32 : 36
                            rotation: 90
                            anchors.top: stuff.bottom
                            anchors.topMargin: 4
                            anchors.horizontalCenter: parent.horizontalCenter

                            visible: (currentDevice.hasBattery && currentDevice.deviceBattery >= 0)
                            source: UtilsDeviceSensors.getDeviceBatteryIcon(currentDevice.deviceBattery)
                            fillMode: Image.PreserveAspectCrop
                            color: cccc
                        }

                        Text {
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 12
                            anchors.horizontalCenter: parent.horizontalCenter

                            visible: (currentDevice.weightMode)

                            text: currentDevice.weightMode
                            font.pixelSize: isPhone ? 20 : 22
                            color: cccc
                            opacity: 0.66
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

                        source: "qrc:/assets/icons_material/duotone-schedule-24px.svg"
                        color: cccc
                    }
                    Text {
                        id: textStatus
                        width: status.width - status.spacing - imageStatus.width
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("Loading...")
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
                    return singleColumn ? parent.width : (parent.width - scaleBox.width)
                }
                height: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return (parent.height - scaleBox.height)
                    return singleColumn ? (parent.height - scaleBox.height) : parent.height
                }

                Loader {
                    id: graphLoader
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    asynchronous: true
                    onLoaded: {
                        weightChart.loadGraph()
                        weightChart.updateGraph()
                    }
                }
            }
        }
    }
}
