import QtQuick 2.15

import ThemeEngine 1.0
import DeviceUtils 1.0
import "qrc:/js/UtilsNumber.js" as UtilsNumber
import "qrc:/js/UtilsDeviceSensors.js" as UtilsDeviceSensors

Item {
    id: deviceWidget
    implicitWidth: 640
    implicitHeight: 128

    property var boxDevice: pointer
    property bool hasHygro: boxDevice.isPlantSensor

    property bool wideAssMode: (width >= 380) || (isTablet && width >= 480)
    property bool bigAssMode: false
    property bool singleColumn: true

    Connections {
        target: boxDevice
        function onSensorUpdated() { initBoxData() }
        function onSensorsUpdated() { initBoxData() }
        function onCapabilitiesUpdated() { initBoxData() }
        function onStatusUpdated() { updateSensorStatus() }
        function onSettingsUpdated() { updateSensorSettings() }
        function onDataUpdated() { updateSensorData() }
        function onRefreshUpdated() { updateSensorData() }
        function onLimitsUpdated() { updateSensorData() }
    }
    Connections {
        target: ThemeEngine
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
        function onTempUnitChanged() {
            if (loaderIndicators.item) loaderIndicators.item.updateData()
        }
    }

    Component.onCompleted: initBoxData()

    ////////////////////////////////////////////////////////////////////////////

    function initBoxData() {
        // Set icon
        if (boxDevice.isPlantSensor) {
            if (hasHygro) {
                if (boxDevice.deviceName === "ropot" || boxDevice.deviceName === "Parrot pot")
                    imageDevice.source = "qrc:/assets/icons_custom/pot_flower-24px.svg"
                else
                    imageDevice.source = "qrc:/assets/icons_material/outline-local_florist-24px.svg"
            } else {
                if (boxDevice.deviceName === "ropot" || boxDevice.deviceName === "Parrot pot")
                    imageDevice.source = "qrc:/assets/icons_custom/pot_empty-24px.svg"
                else
                    imageDevice.source = "qrc:/assets/icons_material/outline-settings_remote-24px.svg"
            }
        } else if (boxDevice.isThermometer) {
            if (boxDevice.deviceName === "MJ_HT_V1" ||
                boxDevice.deviceName === "ClearGrass Temp & RH" ||
                boxDevice.deviceName === "Qingping Temp & RH M" || boxDevice.deviceName === "Qingping Temp & RH H" ||
                boxDevice.deviceName === "Qingping Temp RH Lite" ||
                boxDevice.deviceName === "ThermoBeacon") {
                imageDevice.source = "qrc:/assets/icons_material/baseline-trip_origin-24px.svg"
            } else if (boxDevice.deviceName === "LYWSD02" ||
                       boxDevice.deviceName === "MHO-C303") {
                imageDevice.source = "qrc:/assets/icons_material/baseline-crop_16_9-24px.svg"
            } else if (boxDevice.deviceName === "LYWSD03MMC" ||
                       boxDevice.deviceName === "MHO-C401" ||
                       boxDevice.deviceName === "XMWSDJO4MMC") {
                imageDevice.source = "qrc:/assets/icons_material/baseline-crop_square-24px.svg"
            } else {
                imageDevice.source = "qrc:/assets/icons_material/outline-settings_remote-24px.svg"
            }
        } else if (boxDevice.isEnvironmentalSensor) {
            if (boxDevice.deviceName === "GeigerCounter") {
                imageDevice.source = "qrc:/assets/icons_custom/nuclear_icon.svg"
            } else {
                imageDevice.source = "qrc:/assets/icons_material/outline-settings_remote-24px.svg"
            }
        } else if (boxDevice.isScale) {
            imageDevice.source = "qrc:/assets/icons_material/baseline-monitor_weight-24px.svg"
        } else if (boxDevice.isMotionSensor) {
            imageDevice.source = "qrc:/assets/icons_material/baseline-sensors-24px.svg"
        } else if (boxDevice.isProbe) {
            imageDevice.source = "qrc:/assets/icons_material/baseline-settings_input_component-24px.svg"
        } else {
            imageDevice.source = "qrc:/assets/icons_material/outline-settings_remote-24px.svg"
        }

        // Load indicators
        if (!loaderIndicators.sourceComponent) {
            if (boxDevice.isPlantSensor) {
                loaderIndicators.sourceComponent = componentPlantSensor
            } else if (boxDevice.isThermometer) {
                if (boxDevice.hasHumiditySensor)
                    loaderIndicators.sourceComponent = componentText_2l
                else
                    loaderIndicators.sourceComponent = componentText_1l
            } else if (boxDevice.isEnvironmentalSensor) {
                if (boxDevice.deviceName === "GeigerCounter")
                    loaderIndicators.sourceComponent = componentText_2l
                else
                    loaderIndicators.sourceComponent = componentEnvironmentalGauge
            } else if (boxDevice.isProbe) {
                if (boxDevice.hasProbesTPMS)
                    loaderIndicators.sourceComponent = componentTPMS
                else
                    loaderIndicators.sourceComponent = componentText_3l
            } else if (boxDevice.isScale) {
                if (boxDevice.hasImpedance)
                    loaderIndicators.sourceComponent = componentText_2l
                else
                    loaderIndicators.sourceComponent = componentText_1l
            } else if (boxDevice.isMotionSensor) {
                loaderIndicators.sourceComponent = componentText_2l
            }

            if (loaderIndicators.item) {
                loaderIndicators.item.initData()
                loaderIndicators.item.updateData()
            }
        }

        updateSensorSettings()
        updateSensorStatus()
        updateSensorData()
    }

    function updateSensorStatus() {
        // Text
        textStatus.text = UtilsDeviceSensors.getDeviceStatusText(boxDevice.status)
        textStatus.color = UtilsDeviceSensors.getDeviceStatusColor(boxDevice.status)

        if (boxDevice.status === DeviceUtils.DEVICE_OFFLINE) {
            if (boxDevice.isDataFresh()) {
                if (boxDevice.lastUpdateMin >= 0 && boxDevice.lastUpdateMin <= 1)
                    textStatus.color = Theme.colorGreen
                else
                    textStatus.color = Theme.colorYellow
                textStatus.text = qsTr("Synced")
            } else if (boxDevice.isDataToday()) {
                textStatus.color = Theme.colorYellow
                textStatus.text = qsTr("Synced")
            }
        }
        // Image
        if (!boxDevice.isDataToday()) {
            if (boxDevice.status === DeviceUtils.DEVICE_QUEUED) {
                imageStatus.source = "qrc:/assets/icons_material/duotone-settings_bluetooth-24px.svg"
                refreshAnimation.running = false
            } else if (boxDevice.status === DeviceUtils.DEVICE_CONNECTING) {
                imageStatus.source = "qrc:/assets/icons_material/duotone-bluetooth_searching-24px.svg"
                refreshAnimation.running = true
            } else if (boxDevice.status === DeviceUtils.DEVICE_CONNECTED) {
                imageStatus.source = "qrc:/assets/icons_material/duotone-bluetooth_connected-24px.svg"
                refreshAnimation.running = true
            } else if (boxDevice.status >= DeviceUtils.DEVICE_WORKING) {
                imageStatus.source = "qrc:/assets/icons_material/duotone-bluetooth_connected-24px.svg"
                refreshAnimation.running = true
            } else {
                imageStatus.source = "qrc:/assets/icons_material/baseline-bluetooth_disabled-24px.svg"
                refreshAnimation.running = false
            }
        }
    }

    function updateSensorSettings() {
        // Title
        if (boxDevice.isPlantSensor) {
            if (boxDevice.deviceAssociatedName !== "")
                textTitle.text = boxDevice.deviceAssociatedName
            else
                textTitle.text = boxDevice.deviceName
        } else if (boxDevice.isThermometer) {
            if (boxDevice.deviceName === "ThermoBeacon")
                textTitle.text = boxDevice.deviceName
            else if (boxDevice.deviceModel)
                textTitle.text = deviceManager.getDeviceModelTheengs(boxDevice.deviceModel)
            else if (boxDevice.deviceName)
                textTitle.text = boxDevice.deviceName
            else
                textTitle.text = qsTr("Thermometer")
        } else if (boxDevice.isDeviceTheengs) {
            textTitle.text = deviceManager.getDeviceModelTheengs(boxDevice.deviceModel)
        } else {
            textTitle.text = boxDevice.deviceName
        }
        // Location
        textLocation.font.pixelSize = bigAssMode ? 20 : 18
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

    function updateSensorIcon() {
        if (boxDevice.isPlantSensor) {
            if (hasHygro) {
                if (boxDevice.deviceName === "ropot" || boxDevice.deviceName === "Parrot pot")
                    imageDevice.source = "qrc:/assets/icons_custom/pot_flower-24px.svg"
                else
                    imageDevice.source = "qrc:/assets/icons_material/outline-local_florist-24px.svg"
            } else {
                if (boxDevice.deviceName === "ropot" || boxDevice.deviceName === "Parrot pot")
                    imageDevice.source = "qrc:/assets/icons_custom/pot_empty-24px.svg"
                else
                    imageDevice.source = "qrc:/assets/icons_material/outline-settings_remote-24px.svg"
            }
        }
    }

    function updateSensorWarnings() {
        // Warnings icons (for sensors with available data)

        if (boxDevice.isDataToday()) {

            if (boxDevice.isPlantSensor) {

                alarmFreeze.visible = false

                // Extreme temperature notif
                if (boxDevice.temperatureC > 40) {
                    alarmFreeze.visible = true
                    alarmFreeze.color = Theme.colorYellow
                    alarmFreeze.source = "qrc:/assets/icons_material/duotone-wb_sunny-24px.svg"
                } else if (boxDevice.temperatureC <= 2 && boxDevice.temperatureC > -80) {
                    alarmFreeze.visible = true
                    alarmFreeze.source = "qrc:/assets/icons_material/baseline-ac_unit-24px.svg"

                    if (boxDevice.temperatureC <= -4)
                        alarmFreeze.color = Theme.colorRed
                    else if (boxDevice.temperatureC <= -2)
                        alarmFreeze.color = Theme.colorYellow
                    else
                        alarmFreeze.color = Theme.colorBlue
                }

            } else if (boxDevice.isEnvironmentalSensor) {

                alarmVentilate.visible = false
                //alarmRadiation.visible = false
                //alarmWarning.visible = false

                // Air warning
                if ((boxDevice.hasVocSensor && boxDevice.voc > 1000) ||
                    (boxDevice.hasCo2Sensor && boxDevice.co2 > 1500)) {
                    alarmVentilate.visible = true
                    alarmVentilate.color = Theme.colorRed
                } else if ((boxDevice.hasVocSensor && boxDevice.voc > 500) ||
                           (boxDevice.hasCo2Sensor && boxDevice.co2 > 850)) {
                    alarmVentilate.visible = true
                    alarmVentilate.color = Theme.colorYellow
                }

                // Radiation warning
                if (boxDevice.hasGeigerCounter) {
                    if (boxDevice.radioactivityM > 1) {
                        //alarmRadiation.visible = true
                        //if (boxDevice.radioactivityM > 10)
                        //    alarmRadiation.color = Theme.colorRed
                        //else
                        //    alarmRadiation.color = Theme.colorYellow
                    }
                }
            }
        }
    }

    function updateSensorData() {
        updateSensorIcon()
        updateSensorWarnings()
        if (loaderIndicators.item) loaderIndicators.item.updateData()
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

        visible: singleColumn
        color: Theme.colorSeparator
    }

    Rectangle {
        id: deviceWidgetRectangleSeparator
        anchors.fill: deviceWidgetRectangle
        anchors.leftMargin: singleColumn ? -12 : 0
        anchors.rightMargin: singleColumn ? -12 : 0
        anchors.topMargin: singleColumn ? -6 : 0
        anchors.bottomMargin: singleColumn ? -6 : 0

        radius: 4
        border.width: 2
        border.color: singleColumn ? "transparent" : Theme.colorSeparator

        color: boxDevice.selected ? Theme.colorSeparator : Theme.colorDeviceWidget
        Behavior on color { ColorAnimation { duration: 133 } }

        opacity: boxDevice.selected ? 0.5 : (singleColumn ? 0 : 1)
        Behavior on opacity { OpacityAnimator { duration: 133 } }
    }

    ////////////////////////////////////////////////////////////////////////////

    Item {
        id: deviceWidgetRectangle
        anchors.fill: parent
        anchors.margins: 6

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.MiddleButton

            onClicked: (mouse) => {
                if (typeof boxDevice === "undefined" || !boxDevice) return

                if (mouse.button === Qt.LeftButton) {
                    // multi selection
                    if ((mouse.modifiers & Qt.ControlModifier) ||
                        (screenDeviceList.selectionMode)) {
                        if (!boxDevice.selected) {
                            screenDeviceList.selectDevice(index)
                        } else {
                            screenDeviceList.deselectDevice(index)
                        }
                        return
                    }

                    // regular click
                    if (boxDevice.isDataAvailable()) {
                        selectedDevice = boxDevice

                        if (boxDevice.isPlantSensor) {
                            screenDevicePlantSensor.loadDevice(boxDevice)
                            appContent.state = "DevicePlantSensor"
                        } else if (boxDevice.isThermometer) {
                            screenDeviceThermometer.loadDevice(boxDevice)
                            appContent.state = "DeviceThermometer"
                        } else if (boxDevice.isEnvironmentalSensor) {
                            screenDeviceEnvironmental.loadDevice(boxDevice)
                            appContent.state = "DeviceEnvironmental"
                        } else if (boxDevice.isProbe) {
                            screenDeviceProbe.loadDevice(boxDevice)
                            appContent.state = "DeviceProbe"
                        } else if (boxDevice.isScale) {
                            screenDeviceScale.loadDevice(boxDevice)
                            appContent.state = "DeviceScale"
                        } else if (boxDevice.isMotionSensor) {
                            screenDeviceMotionSensor.loadDevice(boxDevice)
                            appContent.state = "DeviceMotionSensor"
                        } else {
                            //
                        }
                    }
                }

                if (mouse.button === Qt.MiddleButton) {
                   // multi selection
                   if (!boxDevice.selected) {
                       screenDeviceList.selectDevice(index)
                   } else {
                       screenDeviceList.deselectDevice(index)
                   }
                   return
                }
            }

            onPressAndHold: {
                // multi selection
                if (!boxDevice.selected) {
                    utilsApp.vibrate(25)
                    screenDeviceList.selectDevice(index)
                } else {
                    screenDeviceList.deselectDevice(index)
                }
            }
        }

        ////////////////

        Row {
            id: rowLeft
            anchors.top: parent.top
            anchors.topMargin: bigAssMode ? 16 : 8
            anchors.left: parent.left
            anchors.leftMargin: bigAssMode ? (singleColumn ? 4 : 16) : (singleColumn ? 6 : 14)
            anchors.right: (rowRight.width > 0) ? rowRight.left : imageStatus.left
            anchors.rightMargin: singleColumn ? 0 : 8
            anchors.bottom: parent.bottom
            anchors.bottomMargin: bigAssMode ? 16 : 8

            spacing: bigAssMode ? (singleColumn ? 20 : 12) : (singleColumn ? 24 : 10)

            IconSvg {
                id: imageDevice
                width: bigAssMode ? 32 : 24
                height: bigAssMode ? 32 : 24
                anchors.verticalCenter: parent.verticalCenter

                color: Theme.colorHighContrast
                visible: (wideAssMode || bigAssMode)
                fillMode: Image.PreserveAspectFit
                asynchronous: true
            }

            Column {
                id: column
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    id: textTitle
                    width: rowLeft.width - imageDevice.width - rowLeft.spacing

                    textFormat: Text.PlainText
                    color: Theme.colorText
                    font.pixelSize: bigAssMode ? 22 : 20
                    //font.capitalization: Font.Capitalize
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                Text {
                    id: textLocation
                    width: rowLeft.width - imageDevice.width - rowLeft.spacing

                    textFormat: Text.PlainText
                    color: Theme.colorSubText
                    font.pixelSize: bigAssMode ? 20 : 18
                    //font.capitalization: Font.Capitalize
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                Row {
                    id: row
                    height: bigAssMode ? 26 : 22
                    anchors.left: parent.left
                    spacing: 8

                    IconSvg {
                        id: imageBattery
                        width: bigAssMode ? 30 : 28
                        height: bigAssMode ? 32 : 30
                        anchors.verticalCenter: parent.verticalCenter

                        visible: (boxDevice.hasBattery && boxDevice.deviceBattery >= 0)
                        source: UtilsDeviceSensors.getDeviceBatteryIcon(boxDevice.deviceBattery)
                        color: Theme.colorIcon
                        rotation: 90
                        fillMode: Image.PreserveAspectCrop
                    }

                    Text {
                        id: textStatus
                        anchors.verticalCenter: parent.verticalCenter

                        textFormat: Text.PlainText
                        color: Theme.colorGreen
                        font.pixelSize: bigAssMode ? 16 : 15

                        SequentialAnimation on opacity {
                            id: opa
                            loops: Animation.Infinite
                            alwaysRunToEnd: true
                            running: (boxDevice.status !== DeviceUtils.DEVICE_OFFLINE &&
                                      boxDevice.status !== DeviceUtils.DEVICE_QUEUED &&
                                      boxDevice.status !== DeviceUtils.DEVICE_CONNECTED)

                            PropertyAnimation { to: 0.33; duration: 750; }
                            PropertyAnimation { to: 1; duration: 750; }
                        }
                    }
                }
            }
        }

        ////////////////

        Row { // alarms icons
            height: 24
            spacing: 8
            anchors.right: rowRight.left
            anchors.rightMargin: 12
            anchors.verticalCenter: rowRight.verticalCenter
            layoutDirection: Qt.RightToLeft

            visible: boxDevice.hasDataToday

            IconSvg {
                id: alarmFreeze
                width: bigAssMode ? 28 : 24
                height: bigAssMode ? 28 : 24
                anchors.verticalCenter: parent.verticalCenter

                visible: false
                source: "qrc:/assets/icons_material/baseline-ac_unit-24px.svg"
                color: Theme.colorYellow
            }
            IconSvg {
                id: alarmVentilate
                width: bigAssMode ? 28 : 24
                height: bigAssMode ? 28 : 24
                anchors.verticalCenter: parent.verticalCenter

                visible: false
                source: "qrc:/assets/icons_material/baseline-air-24px.svg"
                color: Theme.colorYellow
            }
/*
            IconSvg {
                id: alarmRadiation
                width: bigAssMode ? 28 : 24
                height: bigAssMode ? 28 : 24
                anchors.verticalCenter: parent.verticalCenter

                visible: false
                asynchronous: true
                source: "qrc:/assets/icons_custom/nuclear_icon.svg"
                color: Theme.colorYellow
            }
            IconSvg {
                id: alarmWarning
                width: bigAssMode ? 28 : 24
                height: bigAssMode ? 28 : 24
                anchors.verticalCenter: parent.verticalCenter

                visible: false
                asynchronous: true
                source: "qrc:/assets/icons_material/baseline-warning-24px.svg"
                color: Theme.colorYellow
            }
*/
        }

        ////////////////

        Row {
            id: rowRight
            anchors.top: parent.top
            anchors.topMargin: bigAssMode ? 16 : 8
            anchors.bottom: parent.bottom
            anchors.bottomMargin: bigAssMode ? 16 : 8
            anchors.right: parent.right
            anchors.rightMargin: singleColumn ? (wideAssMode ? 0 : -4) : (bigAssMode ? 14 : 10)

            spacing: 8

            ////

            Loader {
                id: loaderIndicators
                anchors.verticalCenter: parent.verticalCenter

                visible: boxDevice.hasDataToday

                sourceComponent: null
                asynchronous: false
            }

            ////

            IconSvg {
                id: imageForward
                width: 32
                height: 32
                anchors.verticalCenter: parent.verticalCenter

                visible: singleColumn
                color: boxDevice.hasData ? Theme.colorHighContrast : Theme.colorSubText
                source: "qrc:/assets/icons_material/baseline-chevron_right-24px.svg"
            }
        }

        ////////////////

        IconSvg {
            id: imageStatus
            width: 32
            height: 32
            anchors.right: parent.right
            anchors.rightMargin: singleColumn ? 56 : 36
            anchors.verticalCenter: parent.verticalCenter

            visible: !boxDevice.hasDataToday
            color: Theme.colorIcon
            opacity: 0.8

            SequentialAnimation on opacity {
                id: refreshAnimation
                loops: Animation.Infinite
                running: false
                alwaysRunToEnd: true
                OpacityAnimator { from: 0.8; to: 0; duration: 750 }
                OpacityAnimator { from: 0; to: 0.8; duration: 750 }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Component {
        id: componentPlantSensor

        Row {
            id: rectangleSensors
            height: rowRight.height

            spacing: 8

            property int sensorWidth: isPhone ? 8 : (bigAssMode ? 12 : 10)
            property int sensorRadius: bigAssMode ? 3 : 2

            function initData() {
                //
            }
            function updateData() {
                //
            }

            Item {
                id: hygro
                width: rectangleSensors.sensorWidth
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                visible: hasHygro

                Rectangle {
                    anchors.fill: parent
                    color: Theme.colorBlue
                    opacity: 0.33
                    radius: rectangleSensors.sensorRadius
                }
                Rectangle {
                    id: hygro_data
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: UtilsNumber.normalize(boxDevice.soilMoisture, boxDevice.limitHygroMin - 1, boxDevice.limitHygroMax) * rowRight.height

                    color: Theme.colorBlue
                    radius: rectangleSensors.sensorRadius
                    Behavior on height { NumberAnimation { duration: 333 } }
                }
            }

            Item {
                id: cond
                width: rectangleSensors.sensorWidth
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                visible: hasHygro

                Rectangle {
                    anchors.fill: parent
                    color: Theme.colorRed
                    opacity: 0.33
                    radius: rectangleSensors.sensorRadius
                }
                Rectangle {
                    id: cond_data
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: UtilsNumber.normalize(boxDevice.soilConductivity, boxDevice.limitConduMin, boxDevice.limitConduMax) * rowRight.height

                    color: Theme.colorRed
                    radius: rectangleSensors.sensorRadius
                    Behavior on height { NumberAnimation { duration: 333 } }
                }
            }

            Item {
                id: temp
                width: rectangleSensors.sensorWidth
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                Rectangle {
                    anchors.fill: parent
                    color: Theme.colorGreen
                    opacity: 0.33
                    radius: rectangleSensors.sensorRadius
                }
                Rectangle {
                    id: temp_data
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: UtilsNumber.normalize(boxDevice.temperatureC, boxDevice.limitTempMin - 1, boxDevice.limitTempMax) * rowRight.height

                    color: Theme.colorGreen
                    radius: rectangleSensors.sensorRadius
                    Behavior on height { NumberAnimation { duration: 333 } }
                }
            }

            Item {
                id: lumi
                width: rectangleSensors.sensorWidth
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                visible: boxDevice.hasLuminositySensor

                Rectangle {
                    anchors.fill: parent
                    color: Theme.colorYellow
                    opacity: 0.33
                    radius: rectangleSensors.sensorRadius
                }
                Rectangle {
                    id: lumi_data
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    height: UtilsNumber.normalize(boxDevice.luminosityLux, boxDevice.limitLuxMin, boxDevice.limitLuxMax) * rowRight.height

                    color: Theme.colorYellow
                    radius: rectangleSensors.sensorRadius
                    Behavior on height { NumberAnimation { duration: 333 } }
                }
            }
        }
    }

    ////////////////

    Component {
        id: componentText_1l

        Row {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4

            function initData() { }

            function updateData() {
                if (boxDevice.isThermometer) {
                    text.text = boxDevice.temperature.toFixed(1)
                    unit.text = "°"
                } else if (boxDevice.isEnvironmentalSensor) {
                    if (boxDevice.hasGeigerCounter) {
                        text.text = boxDevice.radioactivityH.toFixed(2)
                        unit.text = qsTr("µSv/h")
                    }
                } else if (boxDevice.isScale) {
                    text.text = (settingsManager.tempUnit === 'C') ? boxDevice.weight.toFixed(1) : (boxDevice.weight * 2.20462).toFixed(1)
                    unit.text = (settingsManager.tempUnit === 'C') ? qsTr("kg") : qsTr("lbs")
                }
            }

            Text {
                id: text
                anchors.verticalCenter: parent.verticalCenter

                textFormat: Text.PlainText
                color: Theme.colorText
                font.letterSpacing: -1.4
                font.pixelSize: bigAssMode ? 28 : 24
            }
            Text {
                id: unit
                anchors.verticalCenter: parent.verticalCenter

                textFormat: Text.PlainText
                color: Theme.colorSubText
                font.letterSpacing: -1.4
                font.pixelSize: bigAssMode ? 24 : 20
            }
        }
    }

    ////////////////

    Component {
        id: componentText_2l

        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 0

            function initData() { }

            function updateData() {
                if (boxDevice.isThermometer) {
                    textOne.text = boxDevice.temperature.toFixed(1) + "°"
                    if (boxDevice.humidity > 0) textTwo.text = boxDevice.humidity.toFixed(0) + "%"
                } else if (boxDevice.isEnvironmentalSensor) {
                    //
                } else if (boxDevice.isScale) {
                    textOne.font.pixelSize = bigAssMode ? 26 : 24
                    textTwo.font.pixelSize = bigAssMode ? 22 : 20

                    textOne.text = (settingsManager.tempUnit === 'C') ? boxDevice.weight.toFixed(1) + qsTr("kg") : (boxDevice.weight * 2.20462).toFixed(1) + qsTr("lbs")
                    if (boxDevice.impedance > 0) textTwo.text = boxDevice.impedance + " Ω"
                } else if (boxDevice.isMotionSensor) {
                    textOne.font.pixelSize = bigAssMode ? 26 : 24
                    textTwo.font.pixelSize = bigAssMode ? 22 : 20

                    if (boxDevice.hasPresence) {
                        if (boxDevice.presence) textOne.text = qsTr("presence")
                        else textOne.text = qsTr("noone")
                    } else if (boxDevice.hasOpen) {
                        if (boxDevice.open) textOne.text = qsTr("opened")
                        else textOne.text = qsTr("closed")
                    }
                    if (boxDevice.hasLuminositySensor) {
                        textTwo.text = boxDevice.luminosityLux + " " + qsTr("lux")
                    }
                } else if (boxDevice.isProbe) {
                    //
                } else {
                    textOne.text = boxDevice.temperature.toFixed(1) + "°"
                    textTwo.text = boxDevice.humidity.toFixed(0) + "%"
                }
            }

            Text {
                id: textOne
                anchors.right: parent.right

                textFormat: Text.PlainText
                color: Theme.colorText
                font.letterSpacing: -1.4
                font.pixelSize: bigAssMode ? 32 : 28
            }

            Text {
                id: textTwo
                anchors.right: parent.right

                textFormat: Text.PlainText
                color: Theme.colorSubText
                font.pixelSize: bigAssMode ? 26 : 22
            }
        }
    }

    ////////////////

    Component {
        id: componentText_3l

        Column {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: 0

            function initData() { }

            function updateData() { }

            Row {
                anchors.right: parent.right
                spacing: 8
                visible: boxDevice.hasTemperature1

                Text {
                    width: 48
                    text: {
                        if (boxDevice.temperature1 > -40)
                            return boxDevice.temperature1.toFixed(0) + "°"
                        return "-"
                    }
                    textFormat: Text.PlainText
                    color: Theme.colorSubText
                    font.pixelSize: 22
                    horizontalAlignment: Text.AlignHCenter
                }
                Text {
                    width: 48
                    text: {
                        if (boxDevice.temperature2 > -40)
                            return boxDevice.temperature2.toFixed(0) + "°"
                        return "-"
                    }
                    textFormat: Text.PlainText
                    color: Theme.colorSubText
                    font.pixelSize: 22
                    horizontalAlignment: Text.AlignHCenter
                }
            }
            Row {
                anchors.right: parent.right
                spacing: 8
                visible: boxDevice.hasTemperature3

                Text {
                    width: 48
                    text: {
                        if (boxDevice.temperature3 > -40)
                            return boxDevice.temperature3.toFixed(0) + "°"
                        return "-"
                    }
                    textFormat: Text.PlainText
                    color: Theme.colorSubText
                    font.pixelSize: 22
                    horizontalAlignment: Text.AlignHCenter
                }
                Text {
                    width: 48
                    text: {
                        if (boxDevice.temperature4 > -40)
                            return boxDevice.temperature4.toFixed(0) + "°"
                        return "-"
                    }
                    textFormat: Text.PlainText
                    color: Theme.colorSubText
                    font.pixelSize: 22
                    horizontalAlignment: Text.AlignHCenter
                }
            }
            Row {
                anchors.right: parent.right
                spacing: 8
                visible: boxDevice.hasTemperature5

                Text {
                    width: 48
                    text: {
                        if (boxDevice.temperature5 > -40)
                            return boxDevice.temperature5.toFixed(0) + "°"
                        return "-"
                    }
                    textFormat: Text.PlainText
                    color: Theme.colorSubText
                    font.pixelSize: 22
                    horizontalAlignment: Text.AlignHCenter
                }
                Text {
                    width: 48
                    text: {
                        if (boxDevice.temperature6 > -40)
                            return boxDevice.temperature6.toFixed(0) + "°"
                        return "-"
                    }
                    textFormat: Text.PlainText
                    color: Theme.colorSubText
                    font.pixelSize: 22
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }

    ////////////////

    Component {
        id: componentTPMS

        Column {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: 12

            function initData() { }
            function updateData() { }

            Row {
                anchors.right: parent.right
                spacing: 14

                Rectangle {
                    width: 28; height: 28; radius: 28;
                    color: {
                        if (boxDevice.alarm1) return Theme.colorRed
                        else if (boxDevice.temperature1 < -80) return Theme.colorSeparator
                        return Theme.colorGreen
                    }
                }
                Rectangle {
                    width: 28; height: 28; radius: 28;
                    color: {
                        if (boxDevice.alarm2) return Theme.colorRed
                        else if (boxDevice.temperature2 < -80) return Theme.colorSeparator
                        return Theme.colorGreen
                    }
                }
            }
            Row {
                anchors.right: parent.right
                spacing: 14

                Rectangle {
                    width: 28; height: 28; radius: 28;
                    color: {
                        if (boxDevice.alarm3) return Theme.colorRed
                        else if (boxDevice.temperature3 < -80) return Theme.colorSeparator
                        return Theme.colorGreen
                    }
                }
                Rectangle {
                    width: 28; height: 28; radius: 28;
                    color: {
                        if (boxDevice.alarm4) return Theme.colorRed
                        else if (boxDevice.temperature4 < -80) return Theme.colorSeparator
                        return Theme.colorGreen
                    }
                }
            }
        }
    }

    ////////////////

    Component {
        id: componentEnvironmentalGauge

        Item {
            width: rowRight.height
            height: width
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 4

            property string primaryValue: "voc"
            property int limitMin: -1
            property int limitMax: -1

            function initData() {
                if (boxDevice.hasSetting("primary")) {
                    primaryValue = boxDevice.getSetting("primary")
                } else {
                    if (boxDevice.hasVocSensor) primaryValue = "voc"
                    else if (boxDevice.hasCo2Sensor) primaryValue = "co2"
                    else if (boxDevice.hasPM10Sensor) primaryValue = "pm10"
                    else if (boxDevice.hasHchoSensor) primaryValue = "hcho"
                    else if (boxDevice.hasGeigerCounter) primaryValue = "nuclear"
                    else primaryValue = "hygrometer"
                }

                if (primaryValue === "voc") {
                    gaugeLegend.text = qsTr("VOC")
                    gaugeValue.from = 0
                    gaugeValue.to = 1500
                    limitMin = 500
                    limitMax = 1000
                    gaugeValue.value = boxDevice.voc
                } else if (primaryValue === "hcho") {
                    gaugeLegend.text = qsTr("HCHO")
                    gaugeValue.from = 0
                    gaugeValue.to = 1000
                    limitMin = 250
                    limitMax = 750
                    gaugeValue.value = boxDevice.hcho
                } else if (primaryValue === "co2") {
                    gaugeLegend.text = (boxDevice.haseCo2Sensor ? qsTr("eCO2") : qsTr("CO2"))
                    gaugeValue.from = 0
                    gaugeValue.to = 2000
                    limitMin = 850
                    limitMax = 1500
                    gaugeValue.value = boxDevice.co2
                }
            }

            function updateData() {
                if (boxDevice.hasSetting("primary")) {
                    primaryValue = boxDevice.getSetting("primary")
                }

                // value
                if (primaryValue === "voc") gaugeValue.value = boxDevice.voc
                else if (primaryValue === "hcho") gaugeValue.value = boxDevice.hcho
                else if (primaryValue === "co2") gaugeValue.value = boxDevice.co2
                else if (primaryValue === "co") gaugeValue.value = boxDevice.co
                else if (primaryValue === "o2") gaugeValue.value = boxDevice.o2
                else if (primaryValue === "o3") gaugeValue.value = boxDevice.o3
                else if (primaryValue === "no2") gaugeValue.value = boxDevice.no2
                else if (primaryValue === "so2") gaugeValue.value = boxDevice.so2

                // limits
                if (limitMin > 0 && limitMax > 0) {
                    var clr = Theme.colorGreen
                    if (gaugeValue.value > limitMax) clr = Theme.colorRed
                    else if (gaugeValue.value > limitMin) clr = Theme.colorOrange
                    gaugeValue.arcColor = clr
                    gaugeValue.backgroundColor = clr
                }
            }

            Text {
                id: gaugeLegend
                anchors.centerIn: parent

                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeContent
                color: Theme.colorSubText
            }

            ProgressArc {
                id: gaugeValue
                anchors.fill: parent

                arcWidth: isPhone ? 8 : (bigAssMode ? 12 : 10)
                arcSpan: 270

                from: 0
                to: 1500
                value: -1

                background: true
                backgroundOpacity: 0.33
            }
        }
    }
}
