import QtQuick
import QtQuick.Controls

import ComponentLibrary

Item {
    id: backgroundPermissionsScreen
    anchors.fill: parent

    property string entryPoint: "Settings"

    ////////////////////////////////////////////////////////////////////////////

    function loadScreen() {
        // refresh permissions
        utilsApp.checkMobileBackgroundLocationPermission()
        permissionManager.checkExactAlarmPermission()
        permissionManager.checkBatteryOptimizationPermission()

        // change screen
        appContent.state = "BackgroundPermissions"
    }

    function loadScreenFrom(screenname) {
        entryPoint = screenname
        loadScreen()
    }

    function backAction() {
        // Background updates are "on" once the OS-required background location
        // permission is granted; the other rows only improve reliability.
        settingsManager.systray = utilsApp.checkMobileBackgroundLocationPermission()
        appContent.state = entryPoint
    }

    Timer {
        id: refreshPermissions
        interval: 333
        repeat: false
        onTriggered: {
            utilsApp.checkMobileBackgroundLocationPermission()
            permissionManager.checkExactAlarmPermission()
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Flickable {
        anchors.fill: parent

        contentWidth: -1
        contentHeight: contentColumn.height

        Column {
            id: contentColumn
            anchors.left: parent.left
            anchors.leftMargin: screenPaddingLeft
            anchors.right: parent.right
            anchors.rightMargin: screenPaddingRight

            topPadding: 20
            bottomPadding: 16
            spacing: 8

            ////////

            Item { // Intro
                id: element_intro
                anchors.left: parent.left
                anchors.right: parent.right
                height: 32

                IconSvg {
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter
                    width: 32
                    height: 32

                    opacity: 0.66
                    color: Theme.colorSubText
                    source: "qrc:/IconLibrary/material-symbols/info.svg"
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: appHeader.headerPosition
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Grant the permissions below so Theengs can keep refreshing your sensors while the app is closed.")
                    textFormat: Text.StyledText
                    lineHeight: 0.8
                    wrapMode: Text.WordWrap
                    color: Theme.colorText
                    font.pixelSize: Theme.fontSizeContent
                }
            }

            ////////

            ListSeparatorPadded { height: 16+1 }

            ////////

            Item { // Background location
                height: 24
                anchors.left: parent.left
                anchors.right: parent.right

                RoundButtonIcon {
                    width: 32
                    height: 32
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    property bool validperm: deviceManager.permissionLocationBackground

                    source: (validperm) ? "qrc:/IconLibrary/material-symbols/check.svg" : "qrc:/IconLibrary/material-symbols/close.svg"
                    iconColor: "white"
                    backgroundColor: (validperm) ? Theme.colorSuccess : Theme.colorSubText
                    backgroundVisible: true

                    onClicked: {
                        utilsApp.vibrate(25)
                        utilsApp.getMobileBackgroundLocationPermission()
                        refreshPermissions.start()
                    }
                }

                Text {
                    height: 16
                    anchors.left: parent.left
                    anchors.leftMargin: appHeader.headerPosition
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Background location")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    font.pixelSize: 17
                    color: Theme.colorText
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Text { // Background location legend
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                anchors.right: parent.right
                anchors.rightMargin: 12

                text: qsTr("To scan for Bluetooth Low Energy sensors in the background, Android requires the <b>background location permission</b> — this is an OS-level requirement that applies to all BLE apps. Theengs only reads sensor advertisements; it does <b>not</b> track your location.")
                textFormat: Text.StyledText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            ////////

            ListSeparatorPadded { height: 16+1; visible: (utilsApp.getAndroidSdkVersion() >= 12) }

            ////////

            Item { // Exact alarms
                height: 24
                anchors.left: parent.left
                anchors.right: parent.right

                visible: (utilsApp.getAndroidSdkVersion() >= 12)

                RoundButtonIcon {
                    width: 32
                    height: 32
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    property bool validperm: permissionManager.exactAlarmPermission

                    source: (validperm) ? "qrc:/IconLibrary/material-symbols/check.svg" : "qrc:/IconLibrary/material-symbols/close.svg"
                    iconColor: "white"
                    backgroundColor: (validperm) ? Theme.colorSuccess : Theme.colorSubText
                    backgroundVisible: true

                    onClicked: {
                        utilsApp.vibrate(25)
                        permissionManager.requestExactAlarmPermission()
                        refreshPermissions.start()
                    }
                }

                Text {
                    height: 16
                    anchors.left: parent.left
                    anchors.leftMargin: appHeader.headerPosition
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Exact alarms")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    font.pixelSize: 17
                    color: Theme.colorText
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Text { // Exact alarms legend
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                anchors.right: parent.right
                anchors.rightMargin: 12

                visible: (utilsApp.getAndroidSdkVersion() >= 12)

                text: qsTr("Allow exact alarms so updates fire on time, even when the phone is in deep sleep. Without it, the update interval is only approximated.")
                textFormat: Text.StyledText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            ////////

            ListSeparatorPadded { height: 16+1 }

            ////////

            Item { // Battery optimisation
                height: 24
                anchors.left: parent.left
                anchors.right: parent.right

                RoundButtonIcon {
                    width: 32
                    height: 32
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    property bool validperm: permissionManager.batteryOptimizationPermission

                    source: (validperm) ? "qrc:/IconLibrary/material-symbols/check.svg" : "qrc:/IconLibrary/material-symbols/close.svg"
                    iconColor: "white"
                    backgroundColor: (validperm) ? Theme.colorSuccess : Theme.colorSubText
                    backgroundVisible: true

                    onClicked: {
                        utilsApp.vibrate(25)
                        permissionManager.requestBatteryOptimizationPermission()
                        refreshPermissions.start()
                    }
                }

                Text {
                    height: 16
                    anchors.left: parent.left
                    anchors.leftMargin: appHeader.headerPosition
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Battery optimisation")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    font.pixelSize: 17
                    color: Theme.colorText
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Text { // Battery optimisation legend
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                anchors.right: parent.right
                anchors.rightMargin: 12

                text: qsTr("Some Android phones aggressively pause background apps to save battery, which can interrupt sensor scanning. For reliable updates, exclude Theengs from <b>battery optimisation</b> in the app info screen. On some devices (Xiaomi, OPPO, Huawei…), <b>autolaunch</b> must also be enabled.")
                textFormat: Text.StyledText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            Row {
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                spacing: Theme.componentMargin

                ButtonWireframe {
                    height: 36

                    colorHighlight: Theme.colorPrimary
                    colorBackground: Theme.colorBackground

                    text: qsTr("Application info")
                    source: "qrc:/IconLibrary/material-icons/duotone/tune.svg"
                    sourceSize: 20

                    onClicked: utilsApp.openAndroidAppInfo("com.theengs.app")
                }

                ButtonWireframe {
                    height: 36

                    colorHighlight: Theme.colorPrimary
                    colorBackground: Theme.colorBackground

                    text: qsTr("About battery savers")
                    source: "qrc:/IconLibrary/material-icons/duotone/launch.svg"
                    sourceSize: 20

                    onClicked: Qt.openUrlExternally("https://dontkillmyapp.com/")
                }
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
