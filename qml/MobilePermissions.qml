import QtQuick
import QtQuick.Controls

import ThemeEngine

Item {
    id: permissionsScreen
    anchors.fill: parent

    property string entryPoint: "About"

    ////////////////////////////////////////////////////////////////////////////

    function loadScreen() {
        // refresh permissions
        deviceManager.checkBluetoothPermissions()
        notificationManager.checkNotificationPermissions()

        // change screen
        appContent.state = "AboutPermissions"
    }

    function loadScreenFrom(screenname) {
        entryPoint = screenname
        loadScreen()
    }

    function backAction() {
        screenAbout.loadScreen()
    }

    Timer {
        id: refreshPermissions
        interval: 333
        repeat: false
        onTriggered: {
            deviceManager.checkBluetoothPermissions()
            notificationManager.checkNotificationPermissions()
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

            Item { // Network access
                anchors.left: parent.left
                anchors.right: parent.right
                height: 24

                RoundButtonIcon {
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter
                    width: 32
                    height: 32

                    property bool validperm: true

                    source: (validperm) ? "qrc:/assets/icons_material/baseline-check-24px.svg" : "qrc:/assets/icons_material/baseline-close-24px.svg"
                    iconColor: (validperm) ? "white" : "white"
                    backgroundColor: (validperm) ? Theme.colorSuccess : Theme.colorSubText
                    backgroundVisible: true
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: appHeader.headerPosition
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter
                    height: 16

                    text: qsTr("Network access")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    font.pixelSize: 17
                    color: Theme.colorText
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Text { // Network access legend
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                anchors.right: parent.right
                anchors.rightMargin: Theme.componentMargin

                text: qsTr("Network state and Internet permissions are used to connect to MQTT brokers.")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            ////////

            ListSeparatorPadded { height: 16+1; visible: Qt.platform.os === "android"; }

            ////////

            Item { // Bluetooth control
                anchors.left: parent.left
                anchors.right: parent.right
                height: 24

                visible: (Qt.platform.os === "android")

                RoundButtonIcon {
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter
                    width: 32
                    height: 32

                    property bool validperm: true

                    source: (validperm) ? "qrc:/assets/icons_material/baseline-check-24px.svg" : "qrc:/assets/icons_material/baseline-close-24px.svg"
                    iconColor: (validperm) ? "white" : "white"
                    backgroundColor: (validperm) ? Theme.colorSuccess : Theme.colorSubText
                    backgroundVisible: true
                }

                Text {
                    height: 16
                    anchors.left: parent.left
                    anchors.leftMargin: appHeader.headerPosition
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Bluetooth control")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    font.pixelSize: 17
                    color: Theme.colorText
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Text { // Bluetooth control legend
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                anchors.right: parent.right
                anchors.rightMargin: Theme.componentMargin

                visible: (Qt.platform.os === "android")

                text: qsTr("Theengs can activate your device's Bluetooth in order to operate.")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            ////////

            ListSeparatorPadded { height: 16+1 }

            ////////

            Item { // Bluetooth
                height: 24
                anchors.left: parent.left
                anchors.right: parent.right

                RoundButtonIcon {
                    width: 32
                    height: 32
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    property bool validperm: deviceManager.permissionOS

                    source: (validperm) ? "qrc:/assets/icons_material/baseline-check-24px.svg" : "qrc:/assets/icons_material/baseline-close-24px.svg"
                    iconColor: (validperm) ? "white" : "white"
                    backgroundColor: (validperm) ? Theme.colorSuccess : Theme.colorSubText
                    backgroundVisible: true

                    onClicked: {
                        utilsApp.vibrate(25)
                        if (Qt.platform.os === "ios") deviceManager.requestBluetoothPermissions()
                        else utilsApp.getMobileBluetoothPermission()
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

                    text: qsTr("Bluetooth")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    font.pixelSize: 17
                    color: Theme.colorText
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Text { // Bluetooth legend
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                anchors.right: parent.right
                anchors.rightMargin: 12

                text: qsTr("Permission is required to scan for nearby Bluetooth Low Energy sensors.")
                textFormat: Text.StyledText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            ////////

            ListSeparatorPadded { height: 16+1; visible: Qt.platform.os === "android"; }

            ////////

            Item { // Location
                anchors.left: parent.left
                anchors.right: parent.right
                height: 24

                visible: (Qt.platform.os === "android")

                RoundButtonIcon {
                    width: 32
                    height: 32
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    property bool validperm: deviceManager.permissionLocationBLE

                    source: (validperm) ? "qrc:/assets/icons_material/baseline-check-24px.svg" : "qrc:/assets/icons_material/baseline-close-24px.svg"
                    iconColor: (validperm) ? "white" : "white"
                    backgroundColor: (validperm) ? Theme.colorSuccess : Theme.colorSubText
                    backgroundVisible: true

                    onClicked: {
                        utilsApp.vibrate(25)
                        utilsApp.getMobileBleLocationPermission()
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

                    text: qsTr("Location")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    font.pixelSize: 17
                    color: Theme.colorText
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Text { // Location legend
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                anchors.right: parent.right
                anchors.rightMargin: 12

                visible: (Qt.platform.os === "android")

                text: qsTr("The Android operating system requires applications to ask for device location permission in order to scan for nearby Bluetooth Low Energy sensors.") + "<br>" +
                      qsTr("Theengs doesn't use, store nor communicate your location to anyone or anything.")
                textFormat: Text.StyledText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }
            ButtonWireframeIcon {
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                height: 36

                visible: (Qt.platform.os === "android")

                primaryColor: Theme.colorPrimary
                secondaryColor: Theme.colorBackground

                text: qsTr("Official information")
                source: "qrc:/assets/icons_material/duotone-launch-24px.svg"
                sourceSize: 20

                onClicked: {
                    if (utilsApp.getAndroidSdkVersion() >= 12)
                        Qt.openUrlExternally("https://developer.android.com/guide/topics/connectivity/bluetooth/permissions#declare-android12-or-higher")
                    else
                        Qt.openUrlExternally("https://developer.android.com/guide/topics/connectivity/bluetooth/permissions#declare-android11-or-lower")
                }
            }

            Item { // GPS
                height: 24
                anchors.left: parent.left
                anchors.right: parent.right

                visible: (Qt.platform.os === "android")

                RoundButtonIcon {
                    width: 32
                    height: 32
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    property bool validperm: deviceManager.permissionLocationGPS

                    source: (validperm) ? "qrc:/assets/icons_material/baseline-check-24px.svg" : "qrc:/assets/icons_material/baseline-close-24px.svg"
                    iconColor: (validperm) ? "white" : "white"
                    backgroundColor: (validperm) ? Theme.colorSuccess : Theme.colorWarning
                    backgroundVisible: true

                    onClicked: {
                        utilsApp.vibrate(25)
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

                    text: qsTr("GPS")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    font.pixelSize: 17
                    color: Theme.colorText
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Text { // GPS legend
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                anchors.right: parent.right
                anchors.rightMargin: Theme.componentMargin

                visible: (Qt.platform.os === "android")

                text: qsTr("Some Android devices also require the GPS to be turned on for Bluetooth operations.")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            ButtonWireframeIcon {
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                height: 36

                visible: (Qt.platform.os === "android")

                primaryColor: Theme.colorPrimary
                secondaryColor: Theme.colorBackground

                text: qsTr("Location settings")
                source: "qrc:/assets/icons_material/duotone-tune-24px.svg"
                sourceSize: 20

                onClicked: utilsApp.openAndroidLocationSettings()
            }

            ////////

            ListSeparatorPadded { height: 16+1; visible: Qt.platform.os === "android"; }

            ////////

            Item { // Background location
                height: 24
                anchors.left: parent.left
                anchors.right: parent.right

                visible: (Qt.platform.os === "android")

                RoundButtonIcon {
                    width: 32
                    height: 32
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    property bool validperm: deviceManager.permissionLocationBackground

                    source: (validperm) ? "qrc:/assets/icons_material/baseline-check-24px.svg" : "qrc:/assets/icons_material/baseline-close-24px.svg"
                    iconColor: (validperm) ? "white" : "white"
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

                visible: (Qt.platform.os === "android")

                text: qsTr("Similarly, background location permission is needed if you want to automatically get data from the sensors, while the application is not explicitly opened.")
                textFormat: Text.StyledText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            ////////

            ListSeparatorPadded { height: 16+1 }

            ////////

            Item { // Notifications
                height: 24
                anchors.left: parent.left
                anchors.right: parent.right

                visible: (Qt.platform.os === "ios" || utilsApp.getAndroidSdkVersion() >= 13)

                RoundButtonIcon {
                    width: 32
                    height: 32
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    property bool validperm: notificationManager.permissionOS

                    source: (validperm) ? "qrc:/assets/icons_material/baseline-check-24px.svg" : "qrc:/assets/icons_material/baseline-close-24px.svg"
                    iconColor: (validperm) ? "white" : "white"
                    backgroundColor: (validperm) ? Theme.colorSuccess : Theme.colorSubText
                    backgroundVisible: true

                    onClicked: {
                        utilsApp.vibrate(25)
                        utilsApp.getMobileNotificationPermission()
                        refreshPermissions.start()
                    }
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: appHeader.headerPosition
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter
                    height: 16

                    text: qsTr("Notifications")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    font.pixelSize: 17
                    color: Theme.colorText
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Text { // Notifications legend
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                anchors.right: parent.right
                anchors.rightMargin: 12

                visible: (Qt.platform.os === "ios" || utilsApp.getAndroidSdkVersion() >= 13)

                text: qsTr("Permission is required to send notifications.")
                textFormat: Text.StyledText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            ////////

            ListSeparatorPadded { height: 16+1 }

            ////////

            Item {
                id: element_infos
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
                    source: "qrc:/assets/icons_material/baseline-info-24px.svg"
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: appHeader.headerPosition
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Click on the checkmarks to request missing permissions.")
                    textFormat: Text.StyledText
                    lineHeight : 0.8
                    wrapMode: Text.WordWrap
                    color: Theme.colorText
                    font.pixelSize: Theme.fontSizeContent
                }
            }

            Text {
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                anchors.right: parent.right
                anchors.rightMargin: Theme.componentMargin

                visible: (Qt.platform.os === "ios")

                text: qsTr("If it has no effect, you may have previously refused a permission.") + "<br>" +
                      qsTr("You can go to the iOS \"settings\" panel to change a permission manually.")
                textFormat: Text.StyledText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }
            Text {
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                anchors.right: parent.right
                anchors.rightMargin: Theme.componentMargin

                visible: (Qt.platform.os === "android")

                text: qsTr("If it has no effect, you may have previously refused a permission and clicked on \"don't ask again\".") + "<br>" +
                      qsTr("You can go to the Android \"application info\" panel to change a permission manually.")
                textFormat: Text.StyledText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            ButtonWireframeIcon {
                anchors.left: parent.left
                anchors.leftMargin: appHeader.headerPosition
                height: 36

                visible: (Qt.platform.os === "android")

                primaryColor: Theme.colorPrimary
                secondaryColor: Theme.colorBackground

                text: qsTr("Application info")
                source: "qrc:/assets/icons_material/duotone-tune-24px.svg"
                sourceSize: 20

                onClicked: utilsApp.openAndroidAppInfo("com.theengs.app")
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
