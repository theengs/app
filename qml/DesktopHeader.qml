import QtQuick

import ComponentLibrary

Rectangle {
    id: appHeader
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right

    height: headerHeight
    color: Theme.colorHeader
    clip: false
    z: 10

    property int headerHeight: isHdpi ? 60 : 64

    property int headerPosition: 64

    property string headerTitle: "Theengs"

    ////////////////////////////////////////////////////////////////////////////

    signal backButtonClicked()
    signal rightMenuClicked() // compatibility

    signal deviceMacButtonClicked()
    signal deviceRebootButtonClicked()
    signal deviceCalibrateButtonClicked()
    signal deviceWateringButtonClicked()
    signal deviceLedButtonClicked()
    signal deviceRefreshButtonClicked()
    signal deviceRefreshRealtimeButtonClicked()
    signal deviceRefreshHistoryButtonClicked()
    signal deviceClearButtonClicked()

    signal deviceDataButtonClicked()
    signal deviceHistoryButtonClicked()
    signal devicePlantButtonClicked()
    signal deviceSettingsButtonClicked()

    signal refreshButtonClicked()
    signal syncButtonClicked()
    signal scanButtonClicked()
    signal plantsButtonClicked()
    signal settingsButtonClicked()
    signal settingsMqttButtonClicked()
    signal aboutButtonClicked()

    function setActiveDeviceData() {
        menuDeviceData.highlighted = true
        menuDeviceHistory.highlighted = false
        menuDeviceSettings.highlighted = false
    }
    function setActiveDeviceHistory() {
        menuDeviceData.highlighted = false
        menuDeviceHistory.highlighted = true
        menuDeviceSettings.highlighted = false
    }
    function setActiveDevicePlant() {
        menuDeviceData.highlighted = false
        menuDeviceHistory.highlighted = false
        menuDeviceSettings.highlighted = false
    }
    function setActiveDeviceSettings() {
        menuDeviceData.highlighted = false
        menuDeviceHistory.highlighted = false
        menuDeviceSettings.highlighted = true
    }

    function setActiveMenu() {
        if (appContent.state === "Tutorial") {
            title.text = qsTr("Welcome")
            menus.visible = false

            buttonBack.source = "qrc:/IconLibrary/material-symbols/close.svg"
        } else {
            title.text = "Theengs"
            menus.visible = true

            if (appContent.state === "DeviceList") {
                buttonBack.source = "qrc:/assets/logos/logo-greyscale.svg"
            } else {
                buttonBack.source = "qrc:/IconLibrary/material-symbols/arrow_back_ios_new.svg"
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    DragHandler {
        // make that surface draggable
        // also, prevent clicks below this area
        onActiveChanged: if (active) appWindow.startSystemMove()
        target: null
    }

    MouseArea {
        width: 44
        height: 44
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter

        hoverEnabled: (buttonBack.source !== "qrc:/IconLibrary/material-icons/duotone/memory.svg")
        onEntered: { buttonBackBg.opacity = 0.5; }
        onExited: { buttonBackBg.opacity = 0; buttonBack.width = 28; }

        onPressed: buttonBack.width = 24
        onReleased: buttonBack.width = 28
        onClicked: backButtonClicked()

        enabled: (buttonBack.source !== "qrc:/IconLibrary/material-icons/duotone/memory.svg" || wideMode)
        visible: enabled

        Rectangle {
            id: buttonBackBg
            anchors.fill: parent
            radius: height
            z: -1
            color: Theme.colorHeaderHighlight
            opacity: 0
            Behavior on opacity { OpacityAnimator { duration: 333 } }
        }

        IconSvg {
            id: buttonBack
            width: 28
            height: width
            anchors.centerIn: parent

            asynchronous: true
            source: "qrc:/IconLibrary/material-icons/duotone/memory.svg"
            color: Theme.colorHeaderContent
        }
    }

    Text {
        id: title
        anchors.left: parent.left
        anchors.leftMargin: 64
        anchors.verticalCenter: parent.verticalCenter

        visible: wideMode
        text: headerTitle
        font.bold: true
        font.pixelSize: Theme.fontSizeHeader
        color: Theme.colorHeaderContent
    }

    ////////////////////////////////////////////////////////////////////////////

    Row {
        id: menus
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        spacing: isHdpi ? 4 : 12
        visible: true

        // DEVICE ACTIONS //////////

        ButtonCompactable {
            id: buttonThermoChart
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: (appContent.state === "DeviceThermometer")

            source: (settingsManager.graphThermometer === "lines") ? "qrc:/IconLibrary/material-icons/duotone/insert_chart.svg" : "qrc:/IconLibrary/material-symbols/timeline.svg";
            tooltipText: qsTr("Switch graph")
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            onClicked: {
                if (settingsManager.graphThermometer === "lines")
                    settingsManager.graphThermometer = "minmax"
                else
                    settingsManager.graphThermometer = "lines"
            }
        }
        ButtonCompactable {
            id: buttonWatering
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: (deviceManager.bluetooth &&
                      (selectedDevice && selectedDevice.hasWaterTank) &&
                      (appContent.state === "DevicePlantSensor"))

            source: "qrc:/IconLibrary/material-icons/duotone/local_drink.svg"
            tooltipText: qsTr("Watering")
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            onClicked: deviceWateringButtonClicked()
        }
        ButtonCompactable {
            id: buttonMAC
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: (deviceManager.bluetooth && selectedDevice &&
                      (Qt.platform.os === "osx" || Qt.platform.os === "ios") &&
                      (appContent.state === "DevicePlantSensor" ||
                       appContent.state === "DeviceThermometer" ||
                       appContent.state === "DeviceEnvironmental" ||
                       appContent.state === "DeviceProbe" ||
                       appContent.state === "DeviceScale" ||
                       appContent.state === "DeviceMotionSensor" ||
                       appContent.state === "DeviceGeneric"))

            source: "qrc:/IconLibrary/material-symbols/pin.svg"
            tooltipText: qsTr("Set MAC address")
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            onClicked: deviceMacButtonClicked()
        }
        ButtonCompactable {
            id: buttonCalibrate
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: (deviceManager.bluetooth &&
                      (selectedDevice && selectedDevice.hasCalibration) &&
                      (appContent.state === "DevicePlantSensor" ||
                       appContent.state === "DeviceThermometer" ||
                       appContent.state === "DeviceEnvironmental"))

            source: "qrc:/IconLibrary/material-icons/duotone/model_training.svg"
            tooltipText: qsTr("Calibrate")
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            onClicked: deviceCalibrateButtonClicked()
        }
        ButtonCompactable {
            id: buttonReboot
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: (deviceManager.bluetooth &&
                      (selectedDevice && selectedDevice.hasReboot) &&
                      (appContent.state === "DevicePlantSensor" ||
                       appContent.state === "DeviceThermometer" ||
                       appContent.state === "DeviceEnvironmental"))

            source: "qrc:/IconLibrary/material-icons/duotone/restart_alt.svg"
            tooltipText: qsTr("Reboot")
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            onClicked: deviceRebootButtonClicked()
        }
        ButtonCompactable {
            id: buttonLed
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: (deviceManager.bluetooth &&
                      (selectedDevice && selectedDevice.hasLED) &&
                      (appContent.state === "DevicePlantSensor" ||
                       appContent.state === "DeviceThermometer" ||
                       appContent.state === "DeviceEnvironmental"))

            source: "qrc:/IconLibrary/material-icons/duotone/emoji_objects.svg"
            tooltipText: qsTr("Blink LED")
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            onClicked: deviceLedButtonClicked()
        }

        ////////////

        Rectangle { // separator
            anchors.verticalCenter: parent.verticalCenter
            height: 40
            width: Theme.componentBorderWidth
            color: Theme.colorHeaderHighlight
            visible: (!singleColumn &&
                      (buttonThermoChart.visible || buttonWatering.visible ||
                       buttonCalibrate.visible || buttonReboot.visible ||
                       buttonLed.visible) &&
                      (buttonRefreshHistory.visible || buttonRefreshData.visible))
        }

        ////////////

        ButtonCompactable {
            id: buttonRefreshHistory
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: (deviceManager.bluetooth &&
                      (selectedDevice && selectedDevice.hasHistory) &&
                      (appContent.state === "DevicePlantSensor" ||
                       appContent.state === "DeviceThermometer" ||
                       appContent.state === "DeviceEnvironmental"))

            source: "qrc:/IconLibrary/material-icons/duotone/date_range.svg"
            tooltipText: qsTr("Synchronize history")
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            onClicked: deviceRefreshHistoryButtonClicked()
        }
        ButtonCompactable {
            id: buttonClearHistory
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: buttonRefreshHistory.visible

            source: "qrc:/IconLibrary/material-icons/duotone/date_clear.svg"
            tooltipText: qsTr("Clear history")
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            onClicked: deviceClearButtonClicked()
        }

        ////////////

        Rectangle { // separator
            anchors.verticalCenter: parent.verticalCenter
            height: 40
            width: Theme.componentBorderWidth
            color: Theme.colorHeaderHighlight
            visible: (!singleColumn && buttonRefreshHistory.visible)
        }

        ////////////

        ButtonCompactable {
            id: buttonRefreshRealtime
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: (deviceManager.bluetooth &&
                      (selectedDevice && selectedDevice.hasRealTime) &&
                      (appContent.state === "DevicePlantSensor" ||
                       appContent.state === "DeviceThermometer" ||
                       appContent.state === "DeviceEnvironmental"))

            source: "qrc:/IconLibrary/material-icons/duotone/update.svg"
            tooltipText: qsTr("Real time data")
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            onClicked: deviceRefreshRealtimeButtonClicked()
        }
        ButtonCompactable {
            id: buttonRefreshData
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: (deviceManager.bluetooth &&
                      (selectedDevice && selectedDevice.hasBluetoothConnection) &&
                      (appContent.state === "DevicePlantSensor" ||
                       appContent.state === "DeviceThermometer" ||
                       appContent.state === "DeviceEnvironmental"))

            source: "qrc:/IconLibrary/material-symbols/refresh.svg"
            tooltipText: qsTr("Refresh sensor")
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            onClicked: deviceRefreshButtonClicked()

            animation: "rotate"
            animationRunning: selectedDevice ? selectedDevice.updating : false
        }

        ////////////

        Rectangle { // separator
            anchors.verticalCenter: parent.verticalCenter
            height: 40
            width: Theme.componentBorderWidth
            color: Theme.colorHeaderHighlight
            visible: (!singleColumn && appContent.state === "DevicePlantSensor")
        }
        Item { // spacer
            width: 1; height: 1;
            anchors.verticalCenter: parent.verticalCenter
            visible: (appContent.state === "DeviceThermometer" || appContent.state === "DeviceEnvironmental")
        }

        // DEVICE MENU //////////

        Row {
            id: menuDevice
            spacing: 0

            visible: (appContent.state === "DevicePlantSensor")

            DesktopHeaderItem {
                id: menuDeviceData
                width: headerHeight
                height: headerHeight

                source: "qrc:/IconLibrary/material-icons/duotone/insert_chart.svg"
                colorContent: Theme.colorHeaderContent
                colorHighlight: Theme.colorHeaderHighlight

                onClicked: deviceDataButtonClicked()
            }
            DesktopHeaderItem {
                id: menuDeviceHistory
                width: headerHeight
                height: headerHeight

                source: "qrc:/IconLibrary/material-icons/duotone/date_range.svg"
                colorContent: Theme.colorHeaderContent
                colorHighlight: Theme.colorHeaderHighlight

                onClicked: deviceHistoryButtonClicked()
            }
            DesktopHeaderItem {
                id: menuDeviceSettings
                width: headerHeight
                height: headerHeight

                source: "qrc:/IconLibrary/material-icons/duotone/memory.svg"
                colorContent: Theme.colorHeaderContent
                colorHighlight: Theme.colorHeaderHighlight

                onClicked: deviceSettingsButtonClicked()
            }
        }

        // MAIN MENU ACTIONS //////////

        ButtonCompactable {
            id: buttonSort
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: (appContent.state === "DeviceList")
            enabled: visible

            source: "qrc:/IconLibrary/material-symbols/filter_list.svg"
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            function setText() {
                var txt = qsTr("Order by:") + " "
                if (settingsManager.orderBy === "model") {
                    txt += qsTr("sensor model")
                } else if (settingsManager.orderBy === "location") {
                    txt += qsTr("location")
                }
                buttonSort.tooltipText = txt
            }

            Component.onCompleted: buttonSort.setText()
            Connections {
                target: settingsManager
                function onOrderByChanged() { buttonSort.setText() }
                function onAppLanguageChanged() { buttonSort.setText() }
            }

            property var sortmode: {
                if (settingsManager.orderBy === "model") {
                    return 1
                } else { // if (settingsManager.orderBy === "location") {
                    return 0
                }
            }

            onClicked: {
                sortmode++
                if (sortmode > 1) sortmode = 0

                if (sortmode === 0) {
                    settingsManager.orderBy = "location"
                    deviceManager.orderby_location()
                } else if (sortmode === 1) {
                    settingsManager.orderBy = "model"
                    deviceManager.orderby_model()
                }
            }
        }

        Rectangle { // separator
            anchors.verticalCenter: parent.verticalCenter
            height: 40
            width: Theme.componentBorderWidth
            color: Theme.colorHeaderHighlight
            visible: (deviceManager.bluetooth && appContent.state === "DeviceList")
        }

        ButtonCompactable {
            id: buttonScan
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: (deviceManager.bluetooth && menuMain.visible)
            enabled: (!deviceManager.syncing)

            text: qsTr("Search for new sensors")
            tooltipText: text
            source: "qrc:/IconLibrary/material-symbols/search.svg"
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            onClicked: scanButtonClicked()

            animation: "fade"
            animationRunning: deviceManager.scanning
        }
        ButtonCompactable {
            id: buttonRefreshAll
            height: compact ? 36 : 34
            anchors.verticalCenter: parent.verticalCenter

            visible: (deviceManager.bluetooth && appContent.state === "DeviceList")
            enabled: (!deviceManager.syncing)

            text: qsTr("Refresh sensor data")
            tooltipText: text
            source: "qrc:/IconLibrary/material-symbols/autorenew.svg"
            iconColor: Theme.colorHeaderContent
            textColor: Theme.colorHeaderContent
            backgroundColor: Theme.colorHeaderHighlight

            onClicked: refreshButtonClicked()

            animation: {
                if (deviceManager.updating && deviceManager.listening) return "both"
                if (deviceManager.updating) return "rotate"
                if (deviceManager.listening) return "fade"
                return ""
            }
            animationRunning: (deviceManager.updating || deviceManager.listening)
        }

        Rectangle { // separator
            anchors.verticalCenter: parent.verticalCenter
            height: 40
            width: Theme.componentBorderWidth
            color: Theme.colorHeaderHighlight
            visible: (deviceManager.bluetooth && appContent.state === "DeviceList")
        }

        // MAIN MENU //////////

        Row {
            id: menuMain

            visible: (appContent.state === "DeviceList" ||
                      appContent.state === "SettingsMqtt" ||
                      appContent.state === "Settings" ||
                      appContent.state === "About")
            spacing: 0

            DesktopHeaderItem {
                id: menuSensors
                width: headerHeight
                height: headerHeight

                source: "qrc:/IconLibrary/material-icons/duotone/memory.svg"
                colorContent: Theme.colorHeaderContent
                colorHighlight: Theme.colorHeaderHighlight

                highlighted: (appContent.state === "DeviceList")
                onClicked: plantsButtonClicked()
            }
            DesktopHeaderItem {
                id: menuSettingsMqtt
                width: headerHeight
                height: headerHeight

                source: "qrc:/IconLibrary/material-icons/duotone/tap_and_play_black.svg"
                colorContent: Theme.colorHeaderContent
                colorHighlight: Theme.colorHeaderHighlight

                highlighted: (appContent.state === "SettingsMqtt")
                onClicked: settingsMqttButtonClicked()
            }
            DesktopHeaderItem {
                id: menuSettings
                width: headerHeight
                height: headerHeight

                source: "qrc:/IconLibrary/material-symbols/settings.svg"
                colorContent: Theme.colorHeaderContent
                colorHighlight: Theme.colorHeaderHighlight

                highlighted: (appContent.state === "Settings")
                onClicked: settingsButtonClicked()
            }
            DesktopHeaderItem {
                id: menuAbout
                width: headerHeight
                height: headerHeight

                source: "qrc:/IconLibrary/material-symbols/info.svg"
                colorContent: Theme.colorHeaderContent
                colorHighlight: Theme.colorHeaderHighlight

                highlighted: (appContent.state === "About")
                onClicked: aboutButtonClicked()
            }
        }
    }

    ////////////

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        visible: (!headerUnicolor &&
                  appContent.state !== "DeviceThermometer" &&
                  appContent.state !== "DeviceEnvironmental" &&
                  appContent.state !== "DeviceProbe" &&
                  appContent.state !== "DeviceScale" &&
                  appContent.state !== "DeviceMotionSensor" &&
                  appContent.state !== "DeviceGeneric" &&
                  appContent.state !== "Tutorial")

        height: 2
        opacity: 0.33
        color: Theme.colorHeaderHighlight
    }

    ////////////
}
