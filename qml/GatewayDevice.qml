import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import ComponentLibrary
import DeviceUtils
import "qrc:/js/UtilsDeviceSensors.js" as UtilsDeviceSensors

Loader {
    id: deviceGateway

    property var currentGateway: null

    ////////////////

    function loadDevice(clickedGateway) {
        if (typeof clickedGateway === "undefined" || !clickedGateway) return
        //if (!clickedGateway.isGateway) return

        //if (clickedGateway === currentGateway) {
        //    appContent.state = "GatewayDevice"
        //    return
        //}

        // set device
        currentGateway = clickedGateway

        // connect device
        if (deviceManager.bluetooth && currentGateway && currentGateway.status < 3) {
            currentGateway.actionConnect()
        }

        // load screen
        deviceGateway.active = true
        deviceGateway.item.loadDevice()
    }

    function backAction() {
        if (deviceGateway.status === Loader.Ready)
            deviceGateway.item.backAction()
    }

    ////////////////

    active: false
    asynchronous: false

    sourceComponent: Item {
        implicitWidth: 480
        implicitHeight: 720

        focus: parent.focus

        ////////////////////////////////////////////////////////////////////////

        function loadDevice() {
            console.log("deviceGateway // loadDevice() >> " + currentGateway)

            appContent.state = "GatewayDevice"
            deviceGateway.state = ""

            // start onboarding?
            if (!currentGateway.onboarded) {
                gatewayOnboarding.loadSubScreen()
            }
        }

        function backAction() {
            if (deviceGateway.state === "onboarding") {
                gatewayOnboarding.backAction()
                deviceGateway.state = ""
                return
            } else if (deviceGateway.state === "wifi") {
                gatewayWifi.backAction()
                deviceGateway.state = ""
                return
            } else if (deviceGateway.state === "mqtt") {
                gatewayMqtt.backAction()
                deviceGateway.state = ""
                return
            }

            appContent.state = "GatewayList"
        }

        ////////////////////////////////////////////////////////////////////////

        GatewayAuthentication {
            id: gatewayAuthentication
            visible: (deviceGateway.state === "authentication")
        }

        GatewayOnboarding {
            id: gatewayOnboarding
            visible: (deviceGateway.state === "onboarding")
        }

        GatewayWiFi {
            id: gatewayWifi
            visible: (deviceGateway.state === "wifi")
        }

        GatewayMqtt {
            id: gatewayMqtt
            visible: (deviceGateway.state === "mqtt")
        }

        ////////

        PopupGatewayConfirm {
            id: gatewayActionConfirm

            onConfirmed: {
                //
            }
        }

        ////////////////////////////////////////////////////////////////////////

        Flickable {
            anchors.fill: parent

            contentWidth: -1
            contentHeight: singleColumn ? (contentColumn1.height + contentColumn2.height + Theme.componentMargin*3)
                                        : Math.max(contentColumn1.height, contentColumn2.height)

            boundsBehavior: isDesktop ? Flickable.OvershootBounds : Flickable.DragAndOvershootBounds
            ScrollBar.vertical: ScrollBar { visible: false }

            visible: (deviceGateway.state === "")

            Grid {
                id: contentGrid

                anchors.left: parent.left
                anchors.leftMargin: Theme.componentMargin
                anchors.right: parent.right
                anchors.rightMargin: Theme.componentMargin

                topPadding: Theme.componentMargin
                bottomPadding: Theme.componentMargin

                rows: 2
                columns: singleColumn ? 1 : 2
                spacing: Theme.componentMargin

                property int www: (contentGrid.width - Theme.componentMargin*(singleColumn ? 0 : 1)) / (singleColumn ? 1 : 2)

                ////////////////

                Column {
                    id: contentColumn1
                    width: contentGrid.www

                    topPadding: 0
                    bottomPadding: 0
                    spacing: Theme.componentMargin

                    ////////

                    Image {
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.componentMargin * 2 // * -1
                        anchors.right: parent.right
                        anchors.rightMargin: Theme.componentMargin * 2 // * -1

                        //height: Math.min(width, contentColumn1.height*.066)
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/assets/gfx/devices/generic.png"
                    }

                    ////////

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 12

                        ButtonChip {
                            height: 34

                            text: {
                                if (!currentGateway) return ""
                                if (currentGateway.disconnecting) return qsTr("Disconnecting...")
                                if (currentGateway.connecting) return qsTr("Connecting...")
                                if (currentGateway.working) return qsTr("Working...") // DEBUG
                                if (currentGateway.connected) return qsTr("Connected")
                                if (currentGateway.available) return qsTr("Available")
                                return qsTr("Offline")
                            }
                            leftIcon: {
                                if (!currentGateway) return ""
                                if (currentGateway.disconnecting) return "qrc:/IconLibrary/material-symbols/bluetooth_searching.svg"
                                if (currentGateway.connecting) return "qrc:/IconLibrary/material-symbols/bluetooth_searching.svg"
                                if (currentGateway.working) return "qrc:/IconLibrary/material-symbols/bluetooth.svg"
                                if (currentGateway.connected) return "qrc:/IconLibrary/material-symbols/bluetooth_connected.svg"
                                if (currentGateway.available) return "qrc:/IconLibrary/material-symbols/settings_bluetooth.svg"
                                return ""
                            }
                            color: {
                                if (!currentGateway) return Theme.colorGrey
                                if (currentGateway.disconnecting) return Theme.colorBlue
                                if (currentGateway.connecting) return Theme.colorBlue
                                if (currentGateway.working) return Theme.colorBlue
                                if (currentGateway.connected) return Theme.colorBlue
                                if (currentGateway.available) return Theme.colorGreen
                                return Theme.colorOrange
                            }

                            onClicked: {
                                if (deviceManager.bluetooth && currentGateway) {
                                    if (currentGateway.status < 3)
                                        currentGateway.actionConnect()
                                }
                            }
                        }

                        ButtonChip {
                            height: 34

                            text: {
                                if (!currentGateway) return ""
                                if (currentGateway.onboardingStatus === 0) return qsTr("Awaiting onboarding")
                                if (currentGateway.onboardingStatus === 1) return qsTr("Onboarding")
                                if (currentGateway.onboardingStatus === 2) return qsTr("Offline")
                                return currentGateway.onboardingStatus
                            }
                            leftIcon: {
                                if (!currentGateway) return ""
                                if (currentGateway.disconnecting) return "qrc:/IconLibrary/material-symbols/bluetooth_searching.svg"
                                if (currentGateway.connecting) return "qrc:/IconLibrary/material-symbols/bluetooth_searching.svg"
                                if (currentGateway.working) return "qrc:/IconLibrary/material-symbols/bluetooth.svg"
                                if (currentGateway.connected) return "qrc:/IconLibrary/material-symbols/bluetooth_connected.svg"
                                if (currentGateway.available) return "qrc:/IconLibrary/material-symbols/settings_bluetooth.svg"
                                return ""
                            }
                            color: Theme.colorGreen

                            onClicked: {
                                //
                            }
                        }
                    }

                    ////////

                    RowLayout { // DEBUG
                        anchors.left: parent.left
                        anchors.right: parent.right

                        visible: false
                        uniformCellSizes: true

                        ButtonSolid {
                            //Layout.preferredWidth: 1
                            Layout.fillWidth: true

                            text: "connect"
                            color: Theme.colorBlue

                            onClicked: {
                                if (deviceManager.bluetooth && currentGateway)
                                    currentGateway.actionWifiConnect()
                            }
                        }
                        ButtonSolid {
                            //Layout.preferredWidth: 1
                            Layout.fillWidth: true

                            text: "disconnect"
                            color: Theme.colorBlue

                            onClicked: {
                                if (deviceManager.bluetooth && currentGateway)
                                    currentGateway.actionWifiDisconnect()
                            }
                        }

                        ButtonSolid {
                            //Layout.preferredWidth: 1
                            Layout.fillWidth: true

                            text: "status"
                            color: Theme.colorBlue

                            onClicked: {
                                if (deviceManager.bluetooth && currentGateway)
                                    currentGateway.actionStatus()
                            }
                        }
                        ButtonSolid {
                            //Layout.preferredWidth: 1
                            Layout.fillWidth: true

                            text: "networks"
                            color: Theme.colorBlue

                            onClicked: {
                                if (deviceManager.bluetooth && currentGateway)
                                    currentGateway.actionWifiScan()
                            }
                        }
                    }

                    ////////

                    ButtonSolid {
                        //width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter

                        //visible: (currentGateway.onboardingStatus === 0)

                        text: "Start onboarding!"
                        color: Theme.colorGreen

                        onClicked: {
                            gatewayOnboarding.loadSubScreen()
                        }
                    }

                    ////////
                }

                ////////////////

                Column {
                    id: contentColumn2
                    width: contentGrid.www

                    topPadding: 0
                    bottomPadding: 0
                    spacing: Theme.componentMargin

                    ////////

                    Column {
                        width: parent.width
                        spacing: Theme.componentMargin
                        visible: (currentGateway.onboardingStatus >= 2)

                        ////

                        ButtonSolid {
                            width: parent.width
                            text: "Edit Gateway settings"

                            color: Theme.colorGrey

                            onClicked: {
                                //gatewayPwd.loadSubScreen()
                            }
                        }

                        ButtonSolid {
                            width: parent.width
                            text: "Edit WiFi settings"

                            color: Theme.colorGrey

                            onClicked: {
                                gatewayWifi.loadSubScreen()
                            }
                        }

                        ButtonSolid {
                            width: parent.width
                            text: "Edit MQTT settings"

                            color: Theme.colorGrey

                            onClicked: {
                                gatewayMqtt.loadSubScreen()
                            }
                        }

                        ////

                        ButtonSolid {
                            width: parent.width
                            text: qsTr("Erase settings")

                            onClicked: {
                                gatewayActionConfirm.mode = "erase"
                                gatewayActionConfirm.open()
                            }
                        }

                        ButtonSolid {
                            width: parent.width
                            text: qsTr("Reboot")

                            onClicked: {
                                gatewayActionConfirm.mode = "reboot"
                                gatewayActionConfirm.open()
                            }
                        }

                        ////
                    }

                    ////////
                }

                ////////////////
            }
        }

        ////////////////////////////////////////////////////////////////////////
    }

    ////////////////
}
