import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import ComponentLibrary

import DeviceUtils
import BatteryPresetUtils

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

        // connect device
        currentDevice.actionConnect(true)

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

        // disconnect device
        //if (currentDevice) currentDevice.actionDisconnect()
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

            function onPresetUpdated() { updatePreset() }
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

            currentPreset = batteryPresetsManager.getPreset(currentDevice.preset)
            currentInterval = 3

            graphLoader.source = "" // force graph reload
            loadGraph()

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

        function updatePreset() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isBatteryMonitor) return
            //console.log("DeviceBatteryMonitor // updatePreset() >> " + currentDevice)

            currentPreset = batteryPresetsManager.getPreset(currentDevice.preset)
            if (graphLoader.status === Loader.Ready) {
                batteryChart.updatePreset()
            }
        }

        function updateStatusText() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isBatteryMonitor) return
            //console.log("DeviceBatteryMonitor // updateStatusText() >> " + currentDevice)

            // Status
            textStatus.text = UtilsDeviceSensors.getDeviceStatusText(currentDevice.status)

            if (currentDevice.status <= DeviceUtils.DEVICE_AVAILABLE &&
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
                if (currentInterval === 0 || currentDevice.batteryVoltage > 0) {
                    currentInterval = 0
                    graphLoader.source = "charts/ChartBatteryRealTime.qml"
                } else {
                    // respect currentInterval
                    graphLoader.source = "charts/ChartBatteryHistory.qml"
                }
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
            //if (graphLoader.status === Loader.Ready) return batteryChart.isIndicator()
            return false
        }
        function resetHistoryMode() {
            //if (graphLoader.status === Loader.Ready) batteryChart.resetIndicator()
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

                //MouseArea { anchors.fill: parent } // prevent clicks below this area

                ////////

                Item { // indicators
                    width: singleColumn ? batteryBox.height * 0.8 : batteryBox.width * 0.7
                    height: width

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -(appHeader.height * 0.24)

                    ////

                    IconSvg { // sensorDisconnected
                        width: isMobile ? 96 : 128
                        height: isMobile ? 96 : 128
                        anchors.centerIn: parent

                        visible: !currentDevice.hasData
                        source: "qrc:/IconLibrary/material-symbols/bluetooth_disabled.svg"
                        color: cccc
                    }

                    ////

                    Item { // battery indicator (percent)
                        id: indicatorP
                        anchors.fill: parent
                        anchors.margins: 16

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
                            value: currentDevice.batteryPercent

                            background: true
                            backgroundOpacity: 0.33
                        }

                        IconSvg {
                            id: batteryIcon
                            anchors.horizontalCenter: indicatorP.horizontalCenter
                            anchors.verticalCenter: indicatorP.verticalCenter
                            //anchors.verticalCenterOffset: indicatorP.height * 0.02

                            width: indicatorP.width * 0.50
                            height: indicatorP.height * 0.50

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
                                visible: !isMobile

                                text: currentDevice.deviceName
                                color: cccc
                                font.pixelSize: isMobile ? 20 : 24
                                font.bold: true
                            }
                            Row {
                                anchors.horizontalCenter: parent.horizontalCenter
                                spacing: 24

                                Text {
                                    visible: (currentDevice.batteryPercent > 0)
                                    text: currentDevice.batteryPercent + "%"
                                    color: cccc
                                    font.pixelSize: isMobile ? 20 : 24
                                    font.bold: false
                                }
                                Text {
                                    visible: (currentDevice.batteryVoltage > 0)
                                    text: currentDevice.batteryVoltage.toFixed(1) + "V"
                                    color: cccc
                                    font.pixelSize: isMobile ? 20 : 24
                                    font.bold: false
                                }
                            }
                        }
                    }

                    ////

                    Item { // battery indicator (voltage)
                        id: indicatorV
                        anchors.top: indicatorP.bottom
                        anchors.left: indicatorP.left
                        anchors.right: indicatorP.right
                        anchors.margins: 0

                        visible: currentDevice.hasData && (currentDevice.batteryVoltage > 0)

                        property real minV: 4
                        property real maxV: 16
                        property real minL: (currentPreset) ? currentPreset.voltageMin : 10
                        property real maxL: (currentPreset) ? currentPreset.voltageMax : 12

                        ProgressBarThemed {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            opacity: 0.33
                            colorBackground: cccc

                            from: indicatorV.minV
                            to: indicatorV.maxV
                            value: 0

                            Rectangle { //
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                radius: 0 // height

                                x: (parent.width / (indicatorV.maxV - indicatorV.minV)) * (indicatorV.minL - indicatorV.minV)
                                width: (parent.width / (indicatorV.maxV - indicatorV.minV)) * (indicatorV.maxL - indicatorV.minL)

                                Canvas {
                                    anchors.top: parent.bottom
                                    anchors.left: parent.left
                                    anchors.leftMargin: -(width/2)
                                    width: 12
                                    height: 8
                                    opacity: 1
                                    rotation: 180

                                    Connections {
                                        target: Theme
                                        function onCurrentThemeChanged() { indicator.requestPaint() }
                                    }
                                    onPaint: {
                                        var ctx = getContext("2d")
                                        ctx.reset()
                                        ctx.moveTo(0, 0)
                                        ctx.lineTo(width, 0)
                                        ctx.lineTo(width / 2, height)
                                        ctx.closePath()
                                        ctx.fillStyle = cccc
                                        ctx.fill()
                                    }
                                }
                                Canvas {
                                    anchors.top: parent.bottom
                                    anchors.right: parent.right
                                    anchors.rightMargin: -(width/2)
                                    width: 12
                                    height: 8
                                    opacity: 1
                                    rotation: 180

                                    Connections {
                                        target: Theme
                                        function onCurrentThemeChanged() { indicator.requestPaint() }
                                    }
                                    onPaint: {
                                        var ctx = getContext("2d")
                                        ctx.reset()
                                        ctx.moveTo(0, 0)
                                        ctx.lineTo(width, 0)
                                        ctx.lineTo(width / 2, height)
                                        ctx.closePath()
                                        ctx.fillStyle = cccc
                                        ctx.fill()
                                    }
                                }
                            }
                        }
                        ProgressBarThemed {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            opacity: 0.66
                            colorBackground: "transparent"
                            colorForeground: Theme.colorPrimary

                            from: indicatorV.minV
                            to: indicatorV.maxV
                            value: currentDevice.batteryVoltage
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
                    return singleColumn ? parent.width : (parent.width - batteryBox.width)
                }
                height: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return (parent.height - batteryBox.height)
                    return singleColumn ? (parent.height - batteryBox.height) : parent.height
                }

                ////////

                Rectangle {
                    id: presetStuff
                    anchors.left: parent.left
                    anchors.right: parent.right

                    visible: true
                    height: isMobile ? 40 : 48
                    color: Theme.colorForeground
                    z: 2

                    RowLayout {
                        id: presetRow
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                        anchors.right: parent.right
                        anchors.rightMargin: 0

                        height: parent.height
                        spacing: 0

                        property int mmm: Theme.componentMargin * (isPhone ? 0.66 : 2.0)

                        Item { // spacer
                            Layout.preferredWidth: presetRow.mmm
                            Layout.preferredHeight: 32
                        }

                        Rectangle {
                            Layout.preferredHeight: parent.height
                            Layout.preferredWidth: legendPreset.contentWidth + presetRow.mmm

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

                        Item { // spacer
                            visible: !singleColumn
                            Layout.preferredWidth: presetRow.mmm
                            Layout.preferredHeight: 32
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
                                }
                            }

                            onClicked: {
                                popupPresetSelection.open()
                            }
                        }

                        Item { // spacer
                            Layout.fillWidth: singleColumn
                            Layout.preferredWidth: presetRow.mmm
                            Layout.preferredHeight: 32
                        }

                        Rectangle {
                            Layout.preferredWidth: legendInterval.contentWidth + presetRow.mmm
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

                        Item { // spacer
                            visible: !singleColumn
                            Layout.preferredWidth: presetRow.mmm
                            Layout.preferredHeight: 32
                        }

                        SelectorMenu {
                            id: selectorInterval
                            Layout.preferredWidth: width
                            Layout.preferredHeight: 32

                            model: ListModel {
                                id: intervalModel
                                ListElement { idx: 0; txt: qsTr("realtime"); itv: 1; src: ""; sz: 16; }
                                ListElement { idx: 1; txt: qsTr("day"); itv: 1; src: ""; sz: 16; }
                                ListElement { idx: 2; txt: qsTr("week"); itv: 7; src: ""; sz: 16; }
                                ListElement { idx: 3; txt: qsTr("month"); itv: 30; src: ""; sz: 16; }
                            }

                            currentSelection: currentInterval
                            onMenuSelected: (index) => {
                                currentInterval = index
                                var changed = false
                                var maxDays = 30;

                                if (index === 0) {
                                    if (graphLoader.source !== "charts/ChartBatteryRealTime.qml") {
                                        graphLoader.source = "charts/ChartBatteryRealTime.qml"
                                        changed = true
                                    }
                                } else {
                                    maxDays = model.get(index).itv
                                    if (graphLoader.source !== "charts/ChartBatteryHistory.qml") {
                                        graphLoader.source = ""
                                        graphLoader.source = "charts/ChartBatteryHistory.qml"
                                        changed = true
                                    } else {
                                        batteryChart.updateMaxDays(maxDays)
                                        batteryChart.updateGraph()
                                    }
                                }

                                if (changed && graphLoader.asynchronous === false) {
                                    batteryChart.updateMaxDays(maxDays)
                                    batteryChart.loadGraph()
                                    batteryChart.updateGraph()
                                }
                            }
                        }

                        Item { // spacer
                            Layout.fillWidth: !singleColumn
                            Layout.preferredWidth: presetRow.mmm
                            Layout.preferredHeight: 32
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

                    asynchronous: false
                    onLoaded: {
                        console.log("graphLoader::onLoaded()")
                        if (graphLoader.asynchronous) {
                            var maxDays = selectorInterval.model.get(currentInterval).itv
                            batteryChart.updateMaxDays(maxDays)
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
