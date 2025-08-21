import QtQuick

import ComponentLibrary

Rectangle {
    id: appHeader
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right

    height: headerHeight + Math.max(screenPaddingStatusbar, screenPaddingTop)
    color: Theme.colorHeader
    clip: false
    z: 10

    property int headerHeight: 52

    property int headerPosition: 56

    property string headerTitle: "Theengs"

    ////////////////////////////////////////////////////////////////////////////

    property string leftMenuMode: "drawer" // drawer / back / close
    signal leftMenuClicked()

    ////////////////////////////////////////////////////////////////////////////

    property string rightMenuMode: "off" // on / off
    signal rightMenuClicked()

    function rightMenuIsOpen() { return actionMenu.visible; }
    function rightMenuClose() { actionMenu.close(); }

    signal deviceMacButtonClicked()
    signal deviceRebootButtonClicked()
    signal deviceCalibrateButtonClicked()
    signal deviceWateringButtonClicked()
    signal deviceLedButtonClicked()
    signal deviceRefreshButtonClicked()
    signal deviceRefreshRealtimeButtonClicked()
    signal deviceRefreshHistoryButtonClicked()
    signal deviceClearButtonClicked()
    signal deviceDataButtonClicked() // compatibility
    signal deviceHistoryButtonClicked() // compatibility
    signal devicePlantButtonClicked() // compatibility
    signal deviceSettingsButtonClicked() // compatibility

    function setActiveDeviceData() { } // compatibility
    function setActiveDeviceHistory() { } // compatibility
    function setActiveDevicePlant() { } // compatibility
    function setActiveDeviceSettings() { } // compatibility

    ////////////////////////////////////////////////////////////////////////////

    // prevent clicks below this area
    MouseArea { anchors.fill: parent; acceptedButtons: Qt.AllButtons; }

    ActionMenuFixed {
        id: actionMenu

        x: parent.width - actionMenu.width - 12
        y: Math.max(screenPaddingStatusbar, screenPaddingTop) + 16

        onMenuSelected: (index) => {
            //console.log("ActionMenu clicked #" + index)
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Rectangle { // OS statusbar area
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: Math.max(screenPaddingStatusbar, screenPaddingTop)
        color: Theme.colorStatusbar
    }

    Item {
        anchors.fill: parent
        anchors.topMargin: Math.max(screenPaddingStatusbar, screenPaddingTop)

        ////////////

        MouseArea { // left button
            width: headerHeight
            height: headerHeight

            visible: true
            onClicked: leftMenuClicked()

            RippleThemed {
                anchor: parent
                width: parent.width
                height: parent.height

                pressed: parent.pressed
                //active: enabled && parent.containsPress
                color: Qt.rgba(Theme.colorForeground.r, Theme.colorForeground.g, Theme.colorForeground.b, 0.1)
            }

            IconSvg {
                id: leftMenuImg
                anchors.centerIn: parent
                width: (headerHeight / 2)
                height: (headerHeight / 2)

                source: {
                    if (leftMenuMode === "drawer") return "qrc:/IconLibrary/material-symbols/menu.svg"
                    if (leftMenuMode === "close") return "qrc:/IconLibrary/material-symbols/close.svg"
                    return "qrc:/IconLibrary/material-symbols/arrow_back.svg"
                }
                color: Theme.colorHeaderContent
            }
        }

        Text { // header title
            anchors.left: parent.left
            anchors.leftMargin: headerPosition
            anchors.right: rightArea.left
            anchors.rightMargin: 8
            anchors.verticalCenter: parent.verticalCenter

            text: headerTitle
            textFormat: Text.PlainText
            font.bold: true
            font.pixelSize: Theme.fontSizeHeader
            color: Theme.colorHeaderContent
            elide: Text.ElideRight
        }

        ////////////

        Row { // right area
            id: rightArea
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.rightMargin: 4
            anchors.bottom: parent.bottom

            spacing: 4
            visible: true

            Item { // gateway indicator
                width: parent.height
                height: width
                anchors.verticalCenter: parent.verticalCenter
                visible: (appContent.state === "GatewayDevice")

                Rectangle {
                    id: gatewayIndicator
                    width: 20; height: 20; radius: 20;
                    anchors.centerIn: parent
                    color: Theme.colorGreen
                    visible: (screenGatewayDevice.currentGateway && screenGatewayDevice.currentGateway.connected)
                }
            }

            Item { // right indicator
                width: parent.height
                height: width
                anchors.verticalCenter: parent.verticalCenter
                visible: (appContent.state === "DeviceList" || appContent.state === "DeviceBrowser")

                IconSvg {
                    id: workingIndicator
                    width: 24; height: 24;
                    anchors.centerIn: parent

                    source: {
                        if (deviceManager.scanning)
                            return "qrc:/IconLibrary/material-symbols/search.svg"
                        else if (deviceManager.syncing)
                            return "qrc:/assets/icons_custom/duotone-date_all.svg"
                        else if (deviceManager.listening)
                            return "qrc:/IconLibrary/material-symbols/autorenew.svg"
                        else
                            return "qrc:/IconLibrary/material-symbols/autorenew.svg"
                    }
                    color: Theme.colorHeaderContent
                    opacity: 0
                    Behavior on opacity { OpacityAnimator { duration: 333 } }

                    NumberAnimation on rotation { // refreshAnimation (rotate)
                        from: 0
                        to: 360
                        duration: 2000
                        loops: Animation.Infinite
                        easing.type: Easing.Linear
                        running: (deviceManager.updating && !deviceManager.scanning && !deviceManager.syncing)
                        alwaysRunToEnd: true
                        onStarted: workingIndicator.opacity = 1
                        onStopped: workingIndicator.opacity = 0
                    }
                    SequentialAnimation on opacity { // scanAnimation (fade)
                        loops: Animation.Infinite
                        running: (deviceManager.scanning || deviceManager.listening || deviceManager.syncing)
                        onStopped: workingIndicator.opacity = 0
                        PropertyAnimation { to: 1; duration: 750; }
                        PropertyAnimation { to: 0.33; duration: 750; }
                    }
                }
            }

            MouseArea { // right button
                width: headerHeight
                height: headerHeight

                visible: (deviceManager.bluetooth &&
                          (appContent.state === "DevicePlantSensor" || appContent.state === "DeviceThermometer" ||
                           (appContent.state === "DeviceEnvironmental" && selectedDevice.hasBluetoothConnection))) ||
                         ((Qt.platform.os === "osx" || Qt.platform.os === "ios") &&
                          (appContent.state === "DevicePlantSensor" || appContent.state === "DeviceThermometer" || appContent.state === "DeviceEnvironmental" ||
                           appContent.state === "DeviceProbe" || appContent.state === "DeviceScale" || appContent.state === "DeviceMotionSensor" || appContent.state === "DeviceGeneric"))

                onClicked: {
                    rightMenuClicked()
                    actionMenu.open()
                }

                RippleThemed {
                    anchor: parent
                    width: parent.width
                    height: parent.height

                    pressed: parent.pressed
                    //active: enabled && parent.containsPress
                    color: Qt.rgba(Theme.colorForeground.r, Theme.colorForeground.g, Theme.colorForeground.b, 0.1)
                }

                IconSvg {
                    width: (headerHeight / 2)
                    height: (headerHeight / 2)
                    anchors.centerIn: parent

                    source: "qrc:/IconLibrary/material-symbols/more_vert.svg"
                    color: Theme.colorHeaderContent
                }
            }
        }

        ////////////
    }

    ////////////////////////////////////////////////////////////////////////////
}
