import QtQuick
import QtQuick.Controls

import ThemeEngine

DrawerThemed {
    contentItem: Item {

        ////////////////

        Column {
            id: headerColumn
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            z: 5

            ////////

            Rectangle { // statusbar area
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: -1

                height: Math.max(screenPaddingTop, screenPaddingStatusbar)
                color: Theme.colorStatusbar // to be able to read statusbar content
            }

            ////////

            Rectangle { // logo area
                anchors.left: parent.left
                anchors.right: parent.right

                height: 80
                color: Theme.colorBackground

                Image {
                    id: imageHeader
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.verticalCenter: parent.verticalCenter

                    width: 40
                    height: 40
                    source: "qrc:/assets/logos/logo.svg"
                }
                Text {
                    id: textHeader
                    anchors.left: imageHeader.right
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: 2

                    text: "Theengs"
                    color: Theme.colorText
                    font.bold: true
                    font.pixelSize: 22
                }
            }
        }

        ////////////////

        Flickable {
            anchors.top: headerColumn.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            contentWidth: -1
            contentHeight: contentColumn.height

            Column {
                id: contentColumn
                anchors.left: parent.left
                anchors.right: parent.right

                ////////

                ListSeparatorPadded { }

                ////////

                DrawerItem {
                    highlighted: (appContent.state === "DeviceList")
                    text: qsTr("Sensors")
                    iconSource: "qrc:/assets/icons_material/duotone-memory-24px.svg"

                    onClicked: {
                        appContent.state = "DeviceList"
                        appDrawer.close()
                    }
                }

                DrawerItem {
                    highlighted: (appContent.state === "DeviceBrowser")
                    enabled: (deviceManager.bluetooth && deviceManager.bluetoothPermissions)

                    text: qsTr("Device browser")
                    iconSource: "qrc:/assets/icons_material/duotone-memory-24px.svg"

                    onClicked: {
                        screenDeviceBrowser.loadScreen()
                        appDrawer.close()
                    }
                }

                ////////

                ListSeparatorPadded { }

                ////////

                DrawerItem {
                    highlighted: (appContent.state === "Ranges")

                    text: qsTr("Ranges")
                    iconSource: "qrc:/assets/icons_material/baseline-outdoor_grill-24px.svg"

                    onClicked: {
                        screenTemperaturePresetList.loadScreen()
                        appDrawer.close()
                    }
                }

                DrawerItem {
                    highlighted: (appContent.state === "SettingsMqtt")
                    text: qsTr("Integration")
                    iconSource: "qrc:/assets/icons_material/baseline-tap_and_play_black-24px.svg"

                    onClicked: {
                        appContent.state = "SettingsMqtt"
                        appDrawer.close()
                    }
                }

                DrawerItem {
                    highlighted: (appContent.state === "Settings")
                    text: qsTr("Settings")
                    iconSource: "qrc:/assets/icons_material/outline-settings-24px.svg"

                    onClicked: {
                        appContent.state = "Settings"
                        appDrawer.close()
                    }
                }

                DrawerItem {
                    highlighted: (appContent.state === "About" || appContent.state === "AboutPermissions")
                    text: qsTr("About")
                    iconSource: "qrc:/assets/icons_material/outline-info-24px.svg"

                    onClicked: {
                        appContent.state = "About"
                        appDrawer.close()
                    }
                }

                ////////

                ListSeparatorPadded { }

                ////////

                DrawerItem {
                    iconSource: "qrc:/assets/icons_material/baseline-sort-24px.svg"
                    text: {
                        var txt = qsTr("Order by:") + " "
                        if (settingsManager.orderBy === "model") {
                            txt += qsTr("sensor model")
                        } else if (settingsManager.orderBy === "location") {
                            txt += qsTr("location")
                        }
                        return txt
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
                        if (sortmode > 3) sortmode = 0

                        if (sortmode === 0) {
                            settingsManager.orderBy = "location"
                            deviceManager.orderby_location()
                        } else if (sortmode === 1) {
                            settingsManager.orderBy = "model"
                            deviceManager.orderby_model()
                        }
                    }
                }

                ////////

                ListSeparatorPadded { }

                ////////

                DrawerButton {
                    text: qsTr("Refresh sensor data")

                    iconSource: "qrc:/assets/icons_material/baseline-autorenew-24px.svg"
                    iconAnimation: deviceManager.updating ? "rotate" : "fade"
                    iconAnimated: (deviceManager.updating || deviceManager.listening)

                    enabled: (deviceManager.bluetooth && !deviceManager.scanning && deviceManager.hasDevices)

                    onClicked: {
                        if (!deviceManager.scanning) {
                            if (deviceManager.updating) {
                                deviceManager.refreshDevices_stop()
                            } else {
                                deviceManager.refreshDevices_start()
                            }
                            appDrawer.close()
                        }
                    }
                }

                ////////

                DrawerButton {
                    text: qsTr("Search for new sensors")

                    iconSource: "qrc:/assets/icons_material/baseline-search-24px.svg"
                    iconAnimation: "fade"
                    iconAnimated: deviceManager.scanning

                    enabled: (deviceManager.bluetooth)

                    onClicked: {
                        if (deviceManager.scanning) {
                            deviceManager.scanDevices_stop()
                        } else {
                            deviceManager.scanDevices_start()
                        }
                        appDrawer.close()
                    }
                }

                ////////
            }
        }

        ////////////////
    }
}
