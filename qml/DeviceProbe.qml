import QtQuick 2.15
import QtQuick.Controls 2.15

import ThemeEngine 1.0
import DeviceUtils 1.0
import "qrc:/js/UtilsDeviceSensors.js" as UtilsDeviceSensors

Loader {
    id: deviceProbe

    sourceComponent: null
    asynchronous: false

    property var currentDevice: null

    ////////

    function loadDevice(clickedDevice) {
        // set device
        if (typeof clickedDevice === "undefined" || !clickedDevice) return
        if (!clickedDevice.isProbe) return
        if (clickedDevice === currentDevice) return
        currentDevice = clickedDevice

        // load screen
        if (!sourceComponent) {
            sourceComponent = componentDeviceProbe
        }
        deviceProbe.item.loadDevice()
    }

    ////////

    function isHistoryMode() {
        if (sourceComponent) return deviceProbe.item.isHistoryMode()
        return false
    }
    function resetHistoryMode() {
        if (sourceComponent) deviceProbe.item.resetHistoryMode()
    }

////////////////////////////////////////////////////////////////////////////////

Component {
    id: componentDeviceProbe

    Item {
        id: itemDeviceProbe
        width: 480
        height: 720

        focus: parent.focus

        property alias probeChart: graphLoader.item

        property string cccc: headerUnicolor ? Theme.colorHeaderContent : "white"

        ////////////////////////////////////////////////////////////////////////

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
            //console.log("DeviceProbe // loadDevice() >> " + currentDevice)

            graphLoader.source = "" // force graph reload

            loadGraph()
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
                (currentDevice.isDataFresh() || currentDevice.isDataToday())) {
                if (currentDevice.lastUpdateMin <= 1)
                    textStatus.text = qsTr("Synced")
                else
                    textStatus.text = qsTr("Synced %1 ago").arg(currentDevice.lastUpdateStr)
            }
        }

        function loadGraph() {
            //var reload = !(settingsManager.graphThermometer === "lines" && graphLoader.source === "ChartPlantDataAio.qml") ||
            //             !(settingsManager.graphThermometer === "minmax" && graphLoader.source === "ChartThermometerMinMax.qml")

            //if (graphLoader.status != Loader.Ready || reload) {
            //    if (settingsManager.graphThermometer === "lines") {
            //        graphLoader.source = "ChartPlantDataAio.qml"
            //    } else {
            //        graphLoader.source = "ChartThermometerMinMax.qml"
            //    }
            //}

            //if (graphLoader.status == Loader.Ready) {
            //    probeChart.loadGraph()
            //    probeChart.updateGraph()
            //}
        }
        function updateGraph() {
            //if (graphLoader.status == Loader.Ready) probeChart.updateGraph()
        }

        function isHistoryMode() {
            //if (graphLoader.status == Loader.Ready) return probeChart.isIndicator()
            return false
        }
        function resetHistoryMode() {
            //if (graphLoader.status == Loader.Ready) probeChart.resetIndicator()
        }

        ////////////////////////////////////////////////////////////////////////

        Flow {
            anchors.fill: parent

            Rectangle {
                id: tempBox

                property int dimboxw: Math.min(deviceProbe.width * 0.4, isPhone ? 320 : 600)
                property int dimboxh: Math.max(deviceProbe.height * 0.333, isPhone ? 180 : 256)

                width: singleColumn ? parent.width : dimboxw
                height: singleColumn ? dimboxh: parent.height
                color: Theme.colorHeader
                z: 5

                MouseArea { anchors.fill: parent } // prevent clicks below this area

                IconSvg { // sensorDisconnected
                    width: isMobile ? 96 : 128
                    height: isMobile ? 96 : 128
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.height / 2)

                    visible: !currentDevice.hasData
                    source: "qrc:/assets/icons_material/baseline-bluetooth_disabled-24px.svg"
                    color: cccc
                }

                Column { // column BBQ probe ///////////////////////////////////
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.height / 2)
                    spacing: 24

                    visible: (currentDevice.deviceName.includes("BBQ") && currentDevice.hasData)

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 12

                        Text { // deviceName
                            anchors.verticalCenter: parent.verticalCenter
                            text: currentDevice.deviceName
                            font.pixelSize: 28
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

                    Row {
                        spacing: 48

                        Rectangle {
                            width: 180
                            height: 48
                            radius: 8
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                text: "#1"
                                opacity: 0.66
                                font.pixelSize: 20
                                font.bold: false
                                color: cccc
                            }

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: 16
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.verticalCenterOffset: 2

                                visible: (currentDevice.temperature1 > -80)
                                text: currentDevice.temperature1.toFixed(1)
                                font.pixelSize: 30
                                font.bold: false
                                color: cccc

                                Text {
                                    anchors.left: parent.right
                                    anchors.top: parent.top
                                    anchors.topMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                    opacity: 0.66
                                    font.pixelSize: 20
                                    font.bold: false
                                    color: cccc
                                }
                            }
                        }

                        Rectangle {
                            width: 180
                            height: 48
                            radius: 8
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                text: "#2"
                                opacity: 0.66
                                font.pixelSize: 20
                                font.bold: false
                                color: cccc
                            }

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: 16
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.verticalCenterOffset: 2

                                visible: (currentDevice.temperature2 > -80)
                                text: currentDevice.temperature2.toFixed(1)
                                font.pixelSize: 30
                                font.bold: false
                                color: cccc

                                Text {
                                    anchors.left: parent.right
                                    anchors.top: parent.top
                                    anchors.topMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                    opacity: 0.66
                                    font.pixelSize: 20
                                    font.bold: false
                                    color: cccc
                                }
                            }
                        }
                    }

                    Row {
                        spacing: 48

                        Rectangle {
                            width: 180
                            height: 48
                            radius: 8
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                text: "#3"
                                opacity: 0.66
                                font.pixelSize: 20
                                font.bold: false
                                color: cccc
                            }

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: 16
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.verticalCenterOffset: 2

                                visible: (currentDevice.temperature3 > -80)
                                text: currentDevice.temperature3.toFixed(1)
                                font.pixelSize: 30
                                font.bold: false
                                color: cccc

                                Text {
                                    anchors.left: parent.right
                                    anchors.top: parent.top
                                    anchors.topMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                    opacity: 0.66
                                    font.pixelSize: 20
                                    font.bold: false
                                    color: cccc
                                }
                            }
                        }

                        Rectangle {
                            width: 180
                            height: 48
                            radius: 8
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                text: "#4"
                                opacity: 0.66
                                font.pixelSize: 20
                                font.bold: false
                                color: cccc
                            }

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: 16
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.verticalCenterOffset: 2

                                visible: (currentDevice.temperature4 > -80)
                                text: currentDevice.temperature4.toFixed(1)
                                font.pixelSize: 30
                                font.bold: false
                                color: cccc

                                Text {
                                    anchors.left: parent.right
                                    anchors.top: parent.top
                                    anchors.topMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                    opacity: 0.66
                                    font.pixelSize: 20
                                    font.bold: false
                                    color: cccc
                                }
                            }
                        }
                    }

                    Row {
                        spacing: 48

                        Rectangle {
                            width: 180
                            height: 48
                            radius: 8
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                text: "#3"
                                opacity: 0.66
                                font.pixelSize: 20
                                font.bold: false
                                color: cccc
                            }

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: 16
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.verticalCenterOffset: 2

                                visible: (currentDevice.temperature5 > -80)
                                text: currentDevice.temperature5.toFixed(1)
                                font.pixelSize: 30
                                font.bold: false
                                color: cccc

                                Text {
                                    anchors.left: parent.right
                                    anchors.top: parent.top
                                    anchors.topMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                    opacity: 0.66
                                    font.pixelSize: 20
                                    font.bold: false
                                    color: cccc
                                }
                            }
                        }

                        Rectangle {
                            width: 180
                            height: 48
                            radius: 8
                            color: Qt.alpha(cccc, 0.1)
                            border.width: 2
                            border.color: Qt.alpha(cccc, 0.33)

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                text: "#6"
                                opacity: 0.66
                                font.pixelSize: 20
                                font.bold: false
                                color: cccc
                            }

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: 16
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.verticalCenterOffset: 2

                                visible: (currentDevice.temperature6 > -80)
                                text: currentDevice.temperature6.toFixed(1)
                                font.pixelSize: 30
                                font.bold: false
                                color: cccc

                                Text {
                                    anchors.left: parent.right
                                    anchors.top: parent.top
                                    anchors.topMargin: 2

                                    text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                    opacity: 0.66
                                    font.pixelSize: 20
                                    font.bold: false
                                    color: cccc
                                }
                            }
                        }
                    }
                }

                Column { // column TPMS probe //////////////////////////////////
                    id: columnTPMS

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.height / 3)
                    spacing: columnTPMS.tsp

                    visible: (currentDevice.deviceName.includes("TPMS") && currentDevice.hasData)

                    property int tsp: singleColumn ? 48 : 80
                    property int tsz: singleColumn ? 140 : 180

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

                            Column {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: -4
                                anchors.verticalCenter: parent.verticalCenter
                                visible: (currentDevice.temperature1 > -99)

                                Text {
                                    text: (currentDevice.pressure1 / 1000).toFixed(1)
                                    font.bold: false
                                    font.pixelSize: 32
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.leftMargin: 2
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: "bar"
                                        opacity: 0.66
                                        font.pixelSize: 20
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                                Text {
                                    text: currentDevice.temperature1.toFixed(1)
                                    font.pixelSize: 32
                                    font.bold: false
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                        opacity: 0.66
                                        font.pixelSize: 20
                                        font.bold: false
                                        color: cccc
                                    }
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

                            Column {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: -4
                                anchors.verticalCenter: parent.verticalCenter
                                visible: (currentDevice.temperature2 > -99)

                                Text {
                                    text: (currentDevice.pressure2 / 1000).toFixed(1)
                                    font.bold: false
                                    font.pixelSize: 32
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.leftMargin: 2
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: "bar"
                                        opacity: 0.66
                                        font.pixelSize: 20
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                                Text {
                                    text: currentDevice.temperature2.toFixed(1)
                                    font.pixelSize: 32
                                    font.bold: false
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                        opacity: 0.66
                                        font.pixelSize: 20
                                        font.bold: false
                                        color: cccc
                                    }
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

                            Column {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: -4
                                anchors.verticalCenter: parent.verticalCenter
                                visible: (currentDevice.temperature3 > -99)

                                Text {
                                    text: (currentDevice.pressure3 / 1000).toFixed(1)
                                    font.bold: false
                                    font.pixelSize: 32
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.leftMargin: 2
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: "bar"
                                        opacity: 0.66
                                        font.pixelSize: 20
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                                Text {
                                    text: currentDevice.temperature3.toFixed(1)
                                    font.pixelSize: 32
                                    font.bold: false
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                        opacity: 0.66
                                        font.pixelSize: 20
                                        font.bold: false
                                        color: cccc
                                    }
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

                            Column {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.horizontalCenterOffset: -4
                                anchors.verticalCenter: parent.verticalCenter
                                visible: (currentDevice.temperature4 > -99)

                                Text {
                                    text: (currentDevice.pressure4 / 1000).toFixed(1)
                                    font.bold: false
                                    font.pixelSize: 32
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.leftMargin: 2
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: "bar"
                                        opacity: 0.66
                                        font.pixelSize: 20
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                                Text {
                                    text: currentDevice.temperature4.toFixed(1)
                                    font.pixelSize: 32
                                    font.bold: false
                                    color: cccc

                                    Text {
                                        anchors.left: parent.right
                                        anchors.top: parent.top
                                        anchors.topMargin: 2
                                        text: (settingsManager.tempUnit === 'C') ? "°C" : "°F"
                                        opacity: 0.66
                                        font.pixelSize: 20
                                        font.bold: false
                                        color: cccc
                                    }
                                }
                            }
                        }
                    }
                }
                Row {
                    anchors.centerIn: columnTPMS
                    visible: columnTPMS.visible
                    spacing: 12

                    Text { // deviceName
                        anchors.verticalCenter: parent.verticalCenter
                        text: currentDevice.deviceName
                        font.pixelSize: 28
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
                width: singleColumn ? parent.width : (parent.width - tempBox.width)
                height: singleColumn ? (parent.height - tempBox.height) : parent.height

                ItemBannerSync {
                    id: bannersync
                    z: 5
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                }

                Loader {
                    id: graphLoader
                    anchors.top: bannersync.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    asynchronous: true
                    onLoaded: {
                        //probeChart.loadGraph()
                        //probeChart.updateGraph()
                    }
                }
            }
        }
    }
}
}
