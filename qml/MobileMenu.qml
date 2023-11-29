import QtQuick 2.15

import ThemeEngine 1.0

Rectangle {
    id: mobileMenu
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom

    property int hhh: (appWindow.isPhone ? 36 : 48)
    property int hhi: (hhh * 0.666)
    property int hhv: visible ? hhh : 0

    height: hhh + screenPaddingBottom
    color: appWindow.isTablet ? Theme.colorTabletmenu : Theme.colorBackground

    visible: (isTablet && (appContent.state === "DevicePlantSensor" ||
                           appContent.state === "DeviceList" ||
                           appContent.state === "DeviceBrowser" ||
                           appContent.state === "SettingsMqtt" ||
                           appContent.state === "Settings" ||
                           appContent.state === "About")) ||
             (isPhone && screenOrientation === Qt.PortraitOrientation &&
                          (appContent.state === "DevicePlantSensor"))

    ////////////////////////////////////////////////////////////////////////////

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        opacity: 0.5
        visible: !appWindow.isPhone
        color: Theme.colorTabletmenuContent
    }

    // prevent clicks below this area
    MouseArea { anchors.fill: parent; acceptedButtons: Qt.AllButtons; }

    ////////////////////////////////////////////////////////////////////////////

    signal deviceDataButtonClicked()
    signal deviceHistoryButtonClicked()
    signal devicePlantButtonClicked()
    signal deviceSettingsButtonClicked()

    function setActiveDeviceData() {
        menuDeviceData.selected = true
        menuDeviceHistory.selected = false
        menuDeviceSettings.selected = false
    }
    function setActiveDeviceHistory() {
        menuDeviceData.selected = false
        menuDeviceHistory.selected = true
        menuDeviceSettings.selected = false
    }
    function setActiveDeviceSettings() {
        menuDeviceData.selected = false
        menuDeviceHistory.selected = false
        menuDeviceSettings.selected = true
    }

    ////////////////////////////////////////////////////////////////////////////

    Row { // main menu
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -screenPaddingBottom
        spacing: (!appWindow.wideMode || (appWindow.isPhone && utilsScreen.screenSize < 5.0)) ? -10 : 20

        visible: (appContent.state === "DeviceList" ||
                  appContent.state === "DeviceBrowser" ||
                  appContent.state === "SettingsMqtt" ||
                  appContent.state === "Settings" ||
                  appContent.state === "About")

        MobileMenuItem_horizontal {
            id: menuMainView
            height: mobileMenu.hhh

            text: qsTr("Sensors")
            source: "qrc:/assets/icons_material/duotone-memory-24px.svg"
            sourceSize: mobileMenu.hhi
            colorContent: Theme.colorTabletmenuContent
            colorHighlight: Theme.colorTabletmenuHighlight

            highlighted: (appContent.state === "DeviceList")
            onClicked: appContent.state = "DeviceList"
        }
        MobileMenuItem_horizontal {
            id: menuDeviceBrowser
            height: mobileMenu.hhh

            text: qsTr("Device browser")
            source: "qrc:/assets/icons_material/baseline-radar-24px.svg"
            sourceSize: mobileMenu.hhi
            colorContent: Theme.colorTabletmenuContent
            colorHighlight: Theme.colorTabletmenuHighlight

            highlighted: (appContent.state === "DeviceBrowser")
            onClicked: screenDeviceBrowser.loadScreen()
        }
        MobileMenuItem_horizontal {
            id: menuSettingsMqtt
            height: mobileMenu.hhh

            text: qsTr("Mqtt Integration")
            source: "qrc:/assets/icons_material/duotone-tap_and_play_black-24px.svg"
            sourceSize: mobileMenu.hhi
            colorContent: Theme.colorTabletmenuContent
            colorHighlight: Theme.colorTabletmenuHighlight

            highlighted: (appContent.state === "SettingsMqtt")
            onClicked: appContent.state = "SettingsMqtt"
        }
        MobileMenuItem_horizontal {
            id: menuSettings
            height: mobileMenu.hhh

            text: qsTr("Settings")
            source: "qrc:/assets/icons_material/baseline-settings-20px.svg"
            sourceSize: mobileMenu.hhi
            colorContent: Theme.colorTabletmenuContent
            colorHighlight: Theme.colorTabletmenuHighlight

            highlighted: (appContent.state === "Settings")
            onClicked: appContent.state = "Settings"
        }
        MobileMenuItem_horizontal {
            id: menuAbout
            height: mobileMenu.hhh

            text: qsTr("About")
            source: "qrc:/assets/icons_material/outline-info-24px.svg"
            sourceSize: mobileMenu.hhi
            colorContent: Theme.colorTabletmenuContent
            colorHighlight: Theme.colorTabletmenuHighlight

            highlighted: (appContent.state === "About" || appContent.state === "AboutPermissions")
            onClicked: appContent.state = "About"
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Row { // plant care
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -screenPaddingBottom
        spacing: (!wideMode || (isPhone && utilsScreen.screenSize < 5.0)) ? -10 : 20

        visible: (appContent.state === "DevicePlantSensor")

        MobileMenuItem_horizontal {
            id: menuDeviceData
            height: mobileMenu.hhh

            text: qsTr("Data")
            source: "qrc:/assets/icons_material/duotone-insert_chart-24px.svg"
            sourceSize: mobileMenu.hhi

            colorContent: Theme.colorTabletmenuContent
            colorHighlight: Theme.colorTabletmenuHighlight

            //highlighted: (appContent.state === "DevicePlantSensor")
            onClicked: mobileMenu.deviceDataButtonClicked()
        }
        MobileMenuItem_horizontal {
            id: menuDeviceHistory
            height: mobileMenu.hhh

            text: qsTr("History")
            source: "qrc:/assets/icons_material/duotone-date_range-24px.svg"
            sourceSize: mobileMenu.hhi

            colorContent: Theme.colorTabletmenuContent
            colorHighlight: Theme.colorTabletmenuHighlight

            //highlighted: (appContent.state === "DevicePlantSensor")
            onClicked: mobileMenu.deviceHistoryButtonClicked()
        }
        MobileMenuItem_horizontal {
            id: menuDeviceSettings
            height: mobileMenu.hhh

            text: qsTr("Settings")
            source: "qrc:/assets/icons_material/duotone-memory-24px.svg"
            sourceSize: mobileMenu.hhi

            colorContent: Theme.colorTabletmenuContent
            colorHighlight: Theme.colorTabletmenuHighlight

            //highlighted: (appContent.state === "DevicePlantSensor")
            onClicked: mobileMenu.deviceSettingsButtonClicked()
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
