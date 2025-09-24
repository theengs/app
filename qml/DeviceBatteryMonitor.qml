import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import ComponentLibrary

import DeviceUtils
import "qrc:/js/UtilsDeviceSensors.js" as UtilsDeviceSensors

import BatteryPresetUtils
import "qrc:/js/UtilsPresets.js" as UtilsPresets

Loader {
    id: deviceBM

    property var currentDevice: null
    property var currentPreset: null
    property int currentInterval: 0

    ////////

    function loadDevice(clickedDevice) {
        if (typeof clickedDevice === "undefined" || !clickedDevice) return
        if (!clickedDevice.isBatteryMonitor) return

        // set device
        if (currentDevice !== clickedDevice) currentDevice = clickedDevice

        // load screen
        if (!deviceBM.active) deviceBM.active = true
        deviceBM.item.loadDevice()

        // change screen
        appContent.state = "DeviceBatteryMonitor"
    }

    ////////

    function backAction() {
        if (deviceBM.status === Loader.Ready)
            deviceBM.item.backAction()
    }

    ////////////////////////////////////////////////////////////////////////////

    active: false
    asynchronous: false

    sourceComponent: Item {
        id: itemDeviceBatteryMonitor
        implicitWidth: 480
        implicitHeight: 720

        focus: parent.focus

        property alias batteryChart: graphLoader.item

        property string cccc: headerUnicolor ? Qt.lighter(Theme.colorHeaderContent, 1.33) : "white"

        ////////

        Connections {
            target: currentDevice
            function onSensorUpdated() { updateHeader() }
            function onSensorsUpdated() { updateHeader() }
            function onCapabilitiesUpdated() { updateHeader() }
            function onStatusUpdated() { updateHeader() }

            function onDataUpdated() { updateData() }
            function onRefreshUpdated() { updateData() }
            function onHistoryUpdated() { }
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
            console.log("DeviceBatteryMonitor // loadDevice() >> " + currentDevice)

            graphLoader.source = "" // force graph reload
            loadGraph()
/*
            currentPreset = batteryPresetsManager.getPreset(currentDevice.preset)
            currentInterval = currentDevice.realtimeWindow
            if (currentInterval === 60) selectorInterval.currentSelection = 3
            else if (currentInterval === 30) selectorInterval.currentSelection = 2
            else if (currentInterval === 10) selectorInterval.currentSelection = 1
            else selectorInterval.currentSelection = 0
*/
            updateHeader()
            updateData()
        }

        function updateHeader() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isBatteryMonitor) return
            //console.log("DeviceBatteryMonitor // updateHeader() >> " + currentDevice)

            // Status
            updateStatusText()
        }

        function updateData() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isBatteryMonitor) return
            //console.log("DeviceBatteryMonitor // updateData() >> " + currentDevice)
        }

        function updateStatusText() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isBatteryMonitor) return
            //console.log("DeviceBatteryMonitor // updateStatusText() >> " + currentDevice)

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
            if (!currentDevice.isBatteryMonitor) return

            if (graphLoader.status !== Loader.Ready) {
                graphLoader.source = "ChartBatteryRealTime.qml"
            }

            if (graphLoader.status === Loader.Ready && graphLoader.asynchronous === false) {
                batteryChart.loadGraph()
                batteryChart.updateGraph()
            }
        }
        function updateGraph() {
            // handled internaly
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
            if (graphLoader.status === Loader.Ready) return batteryChart.isIndicator()
            return false
        }
        function resetHistoryMode() {
            if (graphLoader.status === Loader.Ready) batteryChart.resetIndicator()
        }

        ////////////////////////////////////////////////////////////////////////

        Flow {
            anchors.fill: parent

            Rectangle {
                id: batteryBox
                property int dimboxw: Math.min(deviceBM.width * 0.4, isPhone ? 300 : 600)
                property int dimboxh: Math.max(deviceBM.height * 0.333, isPhone ? 180 : 256)

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

                Item { // indicators
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.height / 3)

                    width: singleColumn ? batteryBox.height * 0.75 : batteryBox.width * 0.66
                    height: width

                    IconSvg { // sensorDisconnected
                        width: isMobile ? 96 : 128
                        height: isMobile ? 96 : 128
                        anchors.centerIn: parent

                        visible: !currentDevice.hasData
                        source: "qrc:/IconLibrary/material-symbols/bluetooth_disabled.svg"
                        color: cccc
                    }

                    Item { // battery indicator (voltage)
                        id: indicatorV
                        anchors.fill: parent
                        anchors.margins: 0
                    }

                    Item { // battery indicator (percent)
                        id: indicator
                        anchors.fill: parent
                        anchors.margins: 0

                        visible: currentDevice.hasData

                        ProgressArc {
                            id: batteryIndicator
                            anchors.fill: parent

                            backgroundColor: cccc

                            arcColor: {
                                if (value < 25) return Theme.colorRed
                                else if (value < 50) return Theme.colorOrange
                                return Theme.colorBlue
                            }
                            arcWidth: isPhone ? 8 : 16
                            arcSpan: 270

                            valueMin: 0
                            valueMax: 100
                            value: currentDevice.battery1

                            background: true
                            backgroundOpacity: 0.33
                        }

                        IconSvg {
                            id: batteryIcon
                            anchors.horizontalCenter: indicator.horizontalCenter
                            anchors.verticalCenter: indicator.verticalCenter
                            anchors.verticalCenterOffset: 16

                            width: indicator.width * 0.50
                            height: indicator.height * 0.50

                            color: cccc
                            smooth: true
                            opacity: 0.6
                            source: "qrc:/assets/icons_custom/battery_car.svg"
                        }

                        Column {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 8
                            spacing: -4

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter

                                text: currentDevice.deviceName
                                color: cccc
                                font.pixelSize: isMobile ? 20 : 24
                                font.bold: true
                            }
                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter

                                text: currentDevice.battery1 + "%"
                                color: cccc
                                font.pixelSize: isMobile ? 20 : 24
                                font.bold: false
                            }
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

            ////////////////

            Item {
                width: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return parent.width
                    return singleColumn ? parent.width : (parent.width - batteryBox.width)
                }
                height: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return (parent.height - batteryBox.height)
                    return singleColumn ? (parent.height - batteryBox.height) : parent.height
                }

                ////////

                Column {
                    id: presetStuff
                    anchors.left: parent.left
                    anchors.right: parent.right

                    visible: true
                    height: 40
                    z: 2

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
                                    return qsTr("preset")
                                }
                                source: {
                                    if (currentPreset) return UtilsPresets.getBatteryPresetIcon(currentPreset.type)
                                    return "qrc:/IconLibrary/material-icons/duotone/tune.svg"
                                }
                                sourceSize: 20

                                PopupBatteryPresetSelection {
                                    id: popupPresetSelection
                                    onSelected: (name) => {
                                        currentDevice.preset = name
                                        currentPreset = batteryPresetsManager.getPreset(currentDevice.preset)
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

                            ButtonClear {
                                text: "connect"
                                onClicked: {
                                    currentDevice.actionConnect()
                                }
                            }
                            ButtonClear {
                                text: "fakedata"
                                onClicked: {
                                    currentDevice.actionFakeVoltage()
                                }
                            }
                        }
                    }
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
                        console.log("graphLoader::onLoaded()")
                        if (graphLoader.asynchronous) {
                            batteryChart.loadGraph()
                            batteryChart.updateGraph()
                        }
                    }
                }

                ////////
            }

            ////////////////
        }

        ////////////////////////////////////////////////////////////////////////
    }
}
