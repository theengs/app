import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

import ThemeEngine 1.0
import MobileUI 1.0

ApplicationWindow {
    id: appWindow
    minimumWidth: 480
    minimumHeight: 960

    flags: (Qt.platform.os === "android") ? Qt.Window : Qt.Window | Qt.MaximizeUsingFullscreenGeometryHint
    color: Theme.colorBackground
    visible: true

    property bool isHdpi: (utilsScreen.screenDpi > 128)
    property bool isDesktop: (Qt.platform.os !== "ios" && Qt.platform.os !== "android")
    property bool isMobile: (Qt.platform.os === "ios" || Qt.platform.os === "android")
    property bool isPhone: ((Qt.platform.os === "ios" || Qt.platform.os === "android") && (utilsScreen.screenSize < 7.0))
    property bool isTablet: ((Qt.platform.os === "ios" || Qt.platform.os === "android") && (utilsScreen.screenSize >= 7.0))

    property var selectedDevice: null

    // Mobile stuff ////////////////////////////////////////////////////////////

    // 1 = Qt.PortraitOrientation, 2 = Qt.LandscapeOrientation
    // 4 = Qt.InvertedPortraitOrientation, 8 = Qt.InvertedLandscapeOrientation
    property int screenOrientation: Screen.primaryOrientation
    property int screenOrientationFull: Screen.orientation
    onScreenOrientationChanged: handleNotchesTimer.restart()

    property int screenPaddingStatusbar: 0
    property int screenPaddingNotch: 0
    property int screenPaddingLeft: 0
    property int screenPaddingRight: 0
    property int screenPaddingBottom: 0

    Timer {
        id: handleNotchesTimer
        interval: 33
        repeat: false
        onTriggered: handleNotches()
    }

    function handleNotches() {
/*
        console.log("handleNotches()")
        console.log("screen width : " + Screen.width)
        console.log("screen width avail : " + Screen.desktopAvailableWidth)
        console.log("screen height: " + Screen.height)
        console.log("screen height avail: " + Screen.desktopAvailableHeight)
        console.log("screen orientation: " + Screen.orientation)
        console.log("screen orientation (primary): " + Screen.primaryOrientation)
*/
        if (Qt.platform.os !== "ios") return
        if (typeof quickWindow === "undefined" || !quickWindow) {
            handleNotchesTimer.restart()
            return
        }

        // Statusbar text color hack (iOS)
        mobileUI.statusbarTheme = (Theme.themeStatusbar === 0) ? 1 : 0
        mobileUI.statusbarTheme = Theme.themeStatusbar

        // Margins
        var safeMargins = utilsScreen.getSafeAreaMargins(quickWindow)
        if (safeMargins["total"] === safeMargins["top"]) {
            screenPaddingStatusbar = safeMargins["top"]
            screenPaddingNotch = 0
            screenPaddingLeft = 0
            screenPaddingRight = 0
            screenPaddingBottom = 0
        } else if (safeMargins["total"] > 0) {
            if (Screen.orientation === Qt.PortraitOrientation) {
                screenPaddingStatusbar = 20
                screenPaddingNotch = 12
                screenPaddingLeft = 0
                screenPaddingRight = 0
                screenPaddingBottom = 6
            } else if (Screen.orientation === Qt.InvertedPortraitOrientation) {
                screenPaddingStatusbar = 12
                screenPaddingNotch = 20
                screenPaddingLeft = 0
                screenPaddingRight = 0
                screenPaddingBottom = 6
            } else if (Screen.orientation === Qt.LandscapeOrientation) {
                screenPaddingStatusbar = 0
                screenPaddingNotch = 0
                screenPaddingLeft = 32
                screenPaddingRight = 0
                screenPaddingBottom = 0
            } else if (Screen.orientation === Qt.InvertedLandscapeOrientation) {
                screenPaddingStatusbar = 0
                screenPaddingNotch = 0
                screenPaddingLeft = 0
                screenPaddingRight = 32
                screenPaddingBottom = 0
            } else {
                screenPaddingStatusbar = 0
                screenPaddingNotch = 0
                screenPaddingLeft = 0
                screenPaddingRight = 0
                screenPaddingBottom = 0
            }
        } else {
            screenPaddingStatusbar = 0
            screenPaddingNotch = 0
            screenPaddingLeft = 0
            screenPaddingRight = 0
            screenPaddingBottom = 0
        }
/*
        console.log("total:" + safeMargins["total"])
        console.log("top:" + safeMargins["top"])
        console.log("left:" + safeMargins["left"])
        console.log("right:" + safeMargins["right"])
        console.log("bottom:" + safeMargins["bottom"])

        console.log("RECAP screenPaddingStatusbar:" + screenPaddingStatusbar)
        console.log("RECAP screenPaddingNotch:" + screenPaddingNotch)
        console.log("RECAP screenPaddingLeft:" + screenPaddingLeft)
        console.log("RECAP screenPaddingRight:" + screenPaddingRight)
        console.log("RECAP screenPaddingBottom:" + screenPaddingBottom)
*/
    }

    MobileUI {
        id: mobileUI
        property bool isLoading: true

        statusbarTheme: Theme.themeStatusbar
        statusbarColor: isLoading ? "white" : Theme.colorStatusbar
        navbarColor: {
            if (isLoading) return "white"
            return Theme.colorBackground
        }
    }

    MobileHeader {
        id: appHeader
        width: appWindow.width
        anchors.top: appWindow.top
    }

    MobileDrawer {
        id: appDrawer
        width: (appWindow.screenOrientation === Qt.PortraitOrientation || appWindow.width < 480) ? 0.8 * appWindow.width : 0.5 * appWindow.width
        height: appWindow.height
    }

    // Events handling /////////////////////////////////////////////////////////

    Component.onCompleted: {
        handleNotchesTimer.restart()
        mobileUI.isLoading = false
    }

    Connections {
        target: appHeader
        function onLeftMenuClicked() {
            if (appContent.state === "DeviceList") {
                appDrawer.open()
            } else {
                if (appContent.state === "Permissions")
                    appContent.state = "About"
                else
                    appContent.state = "DeviceList"
            }
        }
        function onRightMenuClicked() {
            //
        }

        function onDeviceLedButtonClicked() {
            if (selectedDevice) {
                selectedDevice.actionLedBlink()
            }
        }
        function onDeviceWateringButtonClicked() {
            if (selectedDevice) {
                selectedDevice.actionWatering()
            }
        }
        function onDeviceCalibrateButtonClicked() {
            if (selectedDevice) {
                popupCalibration.open()
            }
        }
        function onDeviceRebootButtonClicked() {
            if (selectedDevice) {
                selectedDevice.actionReboot()
            }
        }

        function onDeviceClearButtonClicked() {
            if (selectedDevice) {
                popupDeleteData.open()
            }
        }
        function onDeviceRefreshHistoryButtonClicked() {
            if (selectedDevice) {
                selectedDevice.refreshStartHistory()
            }
        }

        function onDeviceRefreshRealtimeButtonClicked() {
            if (selectedDevice) {
                selectedDevice.refreshStartRealtime()
            }
        }
        function onDeviceRefreshButtonClicked() {
            if (selectedDevice) {
                deviceManager.updateDevice(selectedDevice.deviceAddress)
            }
        }
    }

    Connections {
        target: Qt.application
        function onStateChanged() {
            switch (Qt.application.state) {
                case Qt.ApplicationSuspended:
                    //console.log("Qt.ApplicationSuspended")
                    deviceManager.refreshDevices_stop()
                    break
                case Qt.ApplicationHidden:
                    //console.log("Qt.ApplicationHidden")
                    deviceManager.refreshDevices_stop()
                    break
                case Qt.ApplicationInactive:
                    //console.log("Qt.ApplicationInactive")
                    break

                case Qt.ApplicationActive:
                    //console.log("Qt.ApplicationActive")

                    // Check if we need an 'automatic' theme change
                    Theme.loadTheme(settingsManager.appTheme)

                    // MQTT
                    mqttManager.reconnect()

                    if (appContent.state === "DeviceBrowser") {
                        // Restart the device browser
                        deviceManager.scanNearby_start()
                    } else {
                        // Listen for nearby devices
                        deviceManager.refreshDevices_listen()
                    }

                    break
            }
        }
    }

    Timer {
        id: exitTimer
        interval: 3333
        running: false
        repeat: false
        onRunningChanged: exitWarning.opacity = running
    }

    // UI sizes ////////////////////////////////////////////////////////////////

    property bool headerUnicolor: (Theme.colorHeader === Theme.colorBackground)

    property bool singleColumn: {
        if (isMobile) {
            if ((isPhone && screenOrientation === Qt.PortraitOrientation) ||
                (isTablet && width < 512)) { // can be a 2/3 split screen on tablet
                return true
            } else {
                return false
            }
        } else {
            return (appWindow.width < appWindow.height)
        }
    }

    property bool wideMode: (isDesktop && width >= 560) || (isTablet && width >= 480)
    property bool wideWideMode: (width >= 640)

    // QML /////////////////////////////////////////////////////////////////////

    PopupCalibration {
        id: popupCalibration
    }
    PopupDeleteData {
        id: popupDeleteData
    }

    FocusScope {
        id: appContent
        anchors.top: appHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        focus: true
        Keys.onBackPressed: {
            if (appContent.state === "DeviceList") {
                if (screenDeviceList.selectionList.length !== 0) {
                    screenDeviceList.exitSelectionMode()
                } else {
                    if (exitTimer.running)
                        Qt.quit()
                    else
                        exitTimer.start()
                }
            } else if (appContent.state === "DevicePlantSensor") {
                if (appHeader.rightMenuIsOpen()) {
                    appHeader.rightMenuClose()
                } else if (screenDevicePlantSensor.isHistoryMode()) {
                    screenDevicePlantSensor.resetHistoryMode()
                } else {
                    appContent.state = "DeviceList"
                }
            } else if (appContent.state === "DeviceThermometer") {
                if (appHeader.rightMenuIsOpen()) {
                    appHeader.rightMenuClose()
                } else if (screenDeviceThermometer.isHistoryMode()) {
                    screenDeviceThermometer.resetHistoryMode()
                } else {
                    appContent.state = "DeviceList"
                }
            } else if (appContent.state === "DeviceEnvironmental") {
                if (appHeader.rightMenuIsOpen()) {
                    appHeader.rightMenuClose()
                } else {
                    appContent.state = "DeviceList"
                }
            } else if (appContent.state === "DeviceProbe") {
                if (appHeader.rightMenuIsOpen()) {
                    appHeader.rightMenuClose()
                } else if (screenDeviceProbe.isHistoryMode()) {
                    screenDeviceProbe.resetHistoryMode()
                } else {
                    appContent.state = "DeviceList"
                }
            } else if (appContent.state === "DeviceScale") {
                if (appHeader.rightMenuIsOpen()) {
                    appHeader.rightMenuClose()
                } else if (screenDeviceScale.isHistoryMode()) {
                    screenDeviceScale.resetHistoryMode()
                } else {
                    appContent.state = "DeviceList"
                }
            } else if (appContent.state === "Permissions") {
                appContent.state = "About"
            } else if (appContent.state === "PlantBrowser") {
                if (screenPlantBrowser.isPlantClicked()) {
                    screenPlantBrowser.backAction()
                } else {
                    appContent.state = "DeviceList"
                }
            } else {
                appContent.state = "DeviceList"
            }
        }

        DeviceList {
            anchors.fill: parent
            anchors.bottomMargin: mobileMenu.hhv
            id: screenDeviceList
        }
        DevicePlantSensor {
            anchors.fill: parent
            anchors.bottomMargin: mobileMenu.height
            id: screenDevicePlantSensor
        }
        DeviceThermometer {
            anchors.fill: parent
            id: screenDeviceThermometer
        }
        DeviceEnvironmental {
            anchors.fill: parent
            id: screenDeviceEnvironmental
        }
        DeviceProbe {
            anchors.fill: parent
            id: screenDeviceProbe
        }
        DeviceScale {
            anchors.fill: parent
            id: screenDeviceScale
        }
        DeviceMotionSensor {
            anchors.fill: parent
            id: screenDeviceMotionSensor
        }
        SettingsMqtt {
            anchors.fill: parent
            anchors.bottomMargin: mobileMenu.hhv
            id: screenSettingsMqtt
        }
        Settings {
            anchors.fill: parent
            anchors.bottomMargin: mobileMenu.hhv
            id: screenSettings
        }
        MobilePermissions {
            anchors.fill: parent
            id: screenPermissions
        }
        About {
            anchors.fill: parent
            anchors.bottomMargin: mobileMenu.hhv
            id: screenAbout
        }

        DeviceBrowser {
            anchors.fill: parent
            id: screenDeviceBrowser
        }

        // Initial state
        state: "DeviceList"

        onStateChanged: {
            screenDeviceList.exitSelectionMode()

            if (state === "DeviceList")
                appHeader.leftMenuMode = "drawer"
            else if (state === "Tutorial")
                appHeader.leftMenuMode = "close"
            else
                appHeader.leftMenuMode = "back"

            if (state === "Tutorial")
                appDrawer.interactive = false
            else
                appDrawer.interactive = true
        }

        states: [
            State {
                name: "DeviceList"
                PropertyChanges { target: appHeader; title: "Theengs"; }
                PropertyChanges { target: screenDeviceList; visible: true; enabled: true; }
                PropertyChanges { target: screenDevicePlantSensor; visible: false; enabled: false }
                PropertyChanges { target: screenDeviceThermometer; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceEnvironmental; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceProbe; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceScale; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceMotionSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenSettingsMqtt; visible: false; enabled: false; }
                PropertyChanges { target: screenSettings; visible: false; enabled: false; }
                PropertyChanges { target: screenPermissions; visible: false; enabled: false; }
                PropertyChanges { target: screenAbout; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceBrowser; visible: false; enabled: false; }
            },
            State {
                name: "DevicePlantSensor"
                PropertyChanges { target: appHeader; title: selectedDevice.deviceName; }
                PropertyChanges { target: screenDeviceList; visible: false; enabled: false; }
                PropertyChanges { target: screenDevicePlantSensor; visible: true; enabled: true; }
                PropertyChanges { target: screenDeviceThermometer; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceEnvironmental; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceProbe; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceScale; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceMotionSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenSettingsMqtt; visible: false; enabled: false; }
                PropertyChanges { target: screenSettings; visible: false; enabled: false; }
                PropertyChanges { target: screenPermissions; visible: false; enabled: false; }
                PropertyChanges { target: screenAbout; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceBrowser; visible: false; enabled: false; }
            },
            State {
                name: "DeviceThermometer"
                PropertyChanges { target: appHeader; title: qsTr("Thermometer"); }
                PropertyChanges { target: screenDeviceList; visible: false; enabled: false; }
                PropertyChanges { target: screenDevicePlantSensor; visible: false; enabled: false }
                PropertyChanges { target: screenDeviceThermometer; visible: true; enabled: true; }
                PropertyChanges { target: screenDeviceEnvironmental; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceProbe; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceScale; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceMotionSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenSettingsMqtt; visible: false; enabled: false; }
                PropertyChanges { target: screenSettings; visible: false; enabled: false; }
                PropertyChanges { target: screenPermissions; visible: false; enabled: false; }
                PropertyChanges { target: screenAbout; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceBrowser; visible: false; enabled: false; }
            },
            State {
                name: "DeviceEnvironmental"
                PropertyChanges { target: appHeader; title: selectedDevice.deviceName; }
                PropertyChanges { target: screenDeviceList; visible: false; enabled: false; }
                PropertyChanges { target: screenDevicePlantSensor; visible: false; enabled: false }
                PropertyChanges { target: screenDeviceThermometer; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceEnvironmental; visible: true; enabled: true; }
                PropertyChanges { target: screenDeviceProbe; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceScale; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceMotionSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenSettingsMqtt; visible: false; enabled: false; }
                PropertyChanges { target: screenSettings; visible: false; enabled: false; }
                PropertyChanges { target: screenPermissions; visible: false; enabled: false; }
                PropertyChanges { target: screenAbout; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceBrowser; visible: false; enabled: false; }
            },
            State {
                name: "DeviceProbe"
                PropertyChanges { target: appHeader; title: selectedDevice.deviceName; }
                PropertyChanges { target: screenDeviceList; visible: false; enabled: false; }
                PropertyChanges { target: screenDevicePlantSensor; visible: false; enabled: false }
                PropertyChanges { target: screenDeviceThermometer; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceEnvironmental; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceProbe; visible: true; enabled: true; }
                PropertyChanges { target: screenDeviceScale; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceMotionSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenSettingsMqtt; visible: false; enabled: false; }
                PropertyChanges { target: screenSettings; visible: false; enabled: false; }
                PropertyChanges { target: screenPermissions; visible: false; enabled: false; }
                PropertyChanges { target: screenAbout; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceBrowser; visible: false; enabled: false; }
            },
            State {
                name: "DeviceScale"
                PropertyChanges { target: appHeader; title: selectedDevice.deviceName; }
                PropertyChanges { target: screenDeviceList; visible: false; enabled: false; }
                PropertyChanges { target: screenDevicePlantSensor; visible: false; enabled: false }
                PropertyChanges { target: screenDeviceThermometer; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceEnvironmental; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceProbe; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceScale; visible: true; enabled: true; }
                PropertyChanges { target: screenDeviceMotionSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenSettingsMqtt; visible: false; enabled: false; }
                PropertyChanges { target: screenSettings; visible: false; enabled: false; }
                PropertyChanges { target: screenPermissions; visible: false; enabled: false; }
                PropertyChanges { target: screenAbout; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceBrowser; visible: false; enabled: false; }
            },
            State {
                name: "DeviceMotionSensor"
                PropertyChanges { target: appHeader; title: selectedDevice.deviceName; }
                PropertyChanges { target: screenDeviceList; visible: false; enabled: false; }
                PropertyChanges { target: screenDevicePlantSensor; visible: false; enabled: false }
                PropertyChanges { target: screenDeviceThermometer; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceEnvironmental; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceProbe; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceScale; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceMotionSensor; visible: true; enabled: true; }
                PropertyChanges { target: screenSettingsMqtt; visible: false; enabled: false; }
                PropertyChanges { target: screenSettings; visible: false; enabled: false; }
                PropertyChanges { target: screenPermissions; visible: false; enabled: false; }
                PropertyChanges { target: screenAbout; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceBrowser; visible: false; enabled: false; }
            },
            State {
                name: "SettingsMqtt"
                PropertyChanges { target: appHeader; title: qsTr("Integration"); }
                PropertyChanges { target: screenDeviceList; visible: false; enabled: false; }
                PropertyChanges { target: screenDevicePlantSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceThermometer; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceEnvironmental; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceProbe; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceScale; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceMotionSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenSettingsMqtt; visible: true; enabled: true; }
                PropertyChanges { target: screenSettings; visible: false; enabled: false; }
                PropertyChanges { target: screenPermissions; visible: false; enabled: false; }
                PropertyChanges { target: screenAbout; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceBrowser; visible: false; enabled: false; }
            },
            State {
                name: "Settings"
                PropertyChanges { target: appHeader; title: qsTr("Settings"); }
                PropertyChanges { target: screenDeviceList; visible: false; enabled: false; }
                PropertyChanges { target: screenDevicePlantSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceThermometer; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceEnvironmental; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceProbe; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceScale; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceMotionSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenSettingsMqtt; visible: false; enabled: false; }
                PropertyChanges { target: screenSettings; visible: true; enabled: true; }
                PropertyChanges { target: screenPermissions; visible: false; enabled: false; }
                PropertyChanges { target: screenAbout; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceBrowser; visible: false; enabled: false; }
            },
            State {
                name: "Permissions"
                PropertyChanges { target: appHeader; title: qsTr("Permissions"); }
                PropertyChanges { target: screenDeviceList; visible: false; enabled: false; }
                PropertyChanges { target: screenDevicePlantSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceThermometer; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceEnvironmental; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceProbe; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceScale; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceMotionSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenSettingsMqtt; visible: false; enabled: false; }
                PropertyChanges { target: screenSettings; visible: false; enabled: false; }
                PropertyChanges { target: screenPermissions; visible: true; enabled: true; }
                PropertyChanges { target: screenAbout; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceBrowser; visible: false; enabled: false; }
            },
            State {
                name: "About"
                PropertyChanges { target: appHeader; title: qsTr("About"); }
                PropertyChanges { target: screenDeviceList; visible: false; enabled: false; }
                PropertyChanges { target: screenDevicePlantSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceThermometer; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceEnvironmental; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceProbe; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceScale; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceMotionSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenSettingsMqtt; visible: false; enabled: false; }
                PropertyChanges { target: screenSettings; visible: false; enabled: false; }
                PropertyChanges { target: screenPermissions; visible: false; enabled: false; }
                PropertyChanges { target: screenAbout; visible: true; enabled: true; }
                PropertyChanges { target: screenDeviceBrowser; visible: false; enabled: false; }
            },
            State {
                name: "DeviceBrowser"
                PropertyChanges { target: appHeader; title: qsTr("Device browser"); }
                PropertyChanges { target: screenDeviceList; visible: false; enabled: false; }
                PropertyChanges { target: screenDevicePlantSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceThermometer; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceEnvironmental; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceProbe; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceScale; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceMotionSensor; visible: false; enabled: false; }
                PropertyChanges { target: screenSettingsMqtt; visible: false; enabled: false; }
                PropertyChanges { target: screenSettings; visible: false; enabled: false; }
                PropertyChanges { target: screenPermissions; visible: false; enabled: false; }
                PropertyChanges { target: screenAbout; visible: false; enabled: false; }
                PropertyChanges { target: screenDeviceBrowser; visible: true; enabled: true; }
            }
        ]
    }

    ////////////////

    MobileMenu {
        id: mobileMenu
    }

    ////////////////

    Rectangle {
        id: exitWarning

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 12

        height: 40
        radius: 4

        color: Theme.colorComponentBackground
        border.color: Theme.colorSeparator
        border.width: 1

        opacity: 0
        Behavior on opacity { OpacityAnimator { duration: 233 } }

        Text {
            anchors.centerIn: parent
            text: qsTr("Press one more time to exit...")
            textFormat: Text.PlainText
            font.pixelSize: Theme.fontSizeContent
            color: Theme.colorText
        }
    }
}
