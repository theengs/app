import QtQuick
import QtQuick.Controls

import ComponentLibrary
import "qrc:/ComponentLibrary/UtilsNumber.js" as UtilsNumber
import "qrc:/js/UtilsDeviceSensors.js" as UtilsDeviceSensors

Item {
    id: devicePlantSensorSettings

    ////////////////////////////////////////////////////////////////////////////

    function updateHeader() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("devicePlantSensorSettings // updateHeader() >> " + currentDevice)
    }

    ////////////////////////////////////////////////////////////////////////////

    property int flow_width: (flow.width - flow.spacing)
    property int flow_divider: Math.round(flow_width / 512)
    property int www: ((flow_width - (flow.spacing * flow_divider)) / flow_divider)

    Flickable {
        anchors.fill: parent

        contentWidth: -1
        contentHeight: flow.height

        Flow {
            id: flow
            anchors.left: parent.left
            anchors.right: parent.right
            height: singleColumn ? maxheight : devicePlantSensorSettings.height

            property int maxheight: 2*topPadding + 3*spacing + itemDevice.height + itemDeviceInfos.height + itemDeviceSensors.height //+ itemDeviceSettings.height

            topPadding: 14
            padding: 12
            bottomPadding: 14
            spacing: 12
            flow: Flow.TopToBottom

            ////////////////////////////////

            Rectangle {
                id: itemDevice
                width: www
                height: itemDeviceContent.height + 24

                radius: Theme.componentRadius
                color: Theme.colorForeground
                border.width: 2
                border.color: Theme.colorSeparator

                IconSvg {
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.bottom: parent.bottom
                    anchors.margins: 0

                    width: parent.height * 0.85
                    height: parent.height * 0.85

                    asynchronous: true
                    smooth: true
                    opacity: 0.15
                    color: Theme.colorSubText
                    fillMode: Image.PreserveAspectFit

                    source: UtilsDeviceSensors.getDeviceImage(currentDevice.deviceName)
                }

                Column {
                    id: itemDeviceContent
                    anchors.top: parent.top
                    anchors.topMargin: 12
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.right: parent.right
                    anchors.rightMargin: 12

                    spacing: 12

                    Column {
                        Text {
                            text: qsTr("Device")
                            textFormat: Text.PlainText
                            color: Theme.colorSubText
                            font.bold: true
                            font.pixelSize: Theme.fontSizeContentVerySmall
                            font.capitalization: Font.AllUppercase
                        }
                        Text {
                            text: currentDevice.deviceName
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContentBig
                            font.capitalization: Font.Capitalize
                            color: Theme.colorHighContrast
                        }
                    }

                    Column {
                        Text {
                            text: qsTr("Address")
                            textFormat: Text.PlainText
                            color: Theme.colorSubText
                            font.bold: true
                            font.pixelSize: Theme.fontSizeContentVerySmall
                            font.capitalization: Font.AllUppercase
                        }
                        Text {
                            text: (Qt.platform.os === "osx" || Qt.platform.os === "ios") ?
                                      currentDevice.deviceAddress :
                                      "[" + currentDevice.deviceAddress + "]"
                            textFormat: Text.PlainText
                            color: Theme.colorHighContrast
                            font.pixelSize: Theme.fontSizeContentBig
                            font.capitalization: Font.AllUppercase
                        }
                    }

                    Column {
                        visible: currentDevice.deviceAddressMAC.length && (Qt.platform.os === "osx" || Qt.platform.os === "ios")

                        Text {
                            text: qsTr("MAC Address")
                            textFormat: Text.PlainText
                            color: Theme.colorSubText
                            font.bold: true
                            font.pixelSize: Theme.fontSizeContentVerySmall
                            font.capitalization: Font.AllUppercase
                        }
                        Text {
                            text: "[" + currentDevice.deviceAddressMAC + "]"
                            textFormat: Text.PlainText
                            color: Theme.colorHighContrast
                            font.pixelSize: Theme.fontSizeContentBig
                            font.capitalization: Font.AllUppercase
                        }
                    }

                    Row {
                        spacing: 32

                        Column {
                            visible: (currentDevice.deviceFirmware)

                            Text {
                                text: qsTr("Firmware")
                                textFormat: Text.PlainText
                                color: Theme.colorSubText
                                font.bold: true
                                font.pixelSize: Theme.fontSizeContentVerySmall
                                font.capitalization: Font.AllUppercase
                            }
                            Text {
                                text: currentDevice.deviceFirmware
                                textFormat: Text.PlainText
                                font.pixelSize: Theme.fontSizeContentBig
                                color: Theme.colorHighContrast

                                IconSvg {
                                    id: imageFwUpdate
                                    width: parent.height - 4; height: parent.height - 4;
                                    anchors.left: parent.right
                                    anchors.leftMargin: 6
                                    anchors.verticalCenter: parent.verticalCenter

                                    source: "qrc:/IconLibrary/material-symbols/new_releases.svg"
                                    color: Theme.colorSubText
                                    visible: !currentDevice.deviceFirmwareUpToDate
                                }
                            }
                        }

                        Column {
                            visible: (currentDevice.hasBattery && currentDevice.deviceBattery >= 0)

                            Text {
                                text: qsTr("Battery")
                                textFormat: Text.PlainText
                                color: Theme.colorSubText
                                font.bold: true
                                font.pixelSize: Theme.fontSizeContentVerySmall
                                font.capitalization: Font.AllUppercase
                            }
                            Text {
                                text: currentDevice.deviceBattery + "%"
                                textFormat: Text.PlainText
                                font.pixelSize: Theme.fontSizeContentBig
                                color: Theme.colorHighContrast

                                IconSvg {
                                    id: imageBattery
                                    width: 32; height: 32;
                                    rotation: 90
                                    anchors.left: parent.right
                                    anchors.leftMargin: 6
                                    anchors.verticalCenter: parent.verticalCenter

                                    source: UtilsDeviceSensors.getDeviceBatteryIcon(currentDevice.deviceBattery)
                                    color: UtilsDeviceSensors.getDeviceBatteryColor(currentDevice.deviceBattery)
                                }
                            }
                        }
                    }

                    Column {
                        visible: uptime.text

                        Text {
                            text: qsTr("Uptime")
                            textFormat: Text.PlainText
                            color: Theme.colorSubText
                            font.bold: true
                            font.pixelSize: Theme.fontSizeContentVerySmall
                            font.capitalization: Font.AllUppercase
                        }
                        Text {
                            id: uptime
                            text: currentDevice.deviceUptime.toLocaleString(Locale.ShortFormat)
                            font.pixelSize: Theme.fontSizeContentBig
                            color: Theme.colorHighContrast
                        }
                    }
/*
                    Column {
                        visible: lastmove.text

                        Text {
                            text: qsTr("Last time moved")
                            textFormat: Text.PlainText
                            color: Theme.colorSubText
                            font.bold: true
                            font.pixelSize: Theme.fontSizeContentVerySmall
                            font.capitalization: Font.AllUppercase
                        }
                        Text {
                            id: lastmove
                            text: currentDevice.lastMove.toLocaleString(Locale.ShortFormat)
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContentBig
                            color: Theme.colorHighContrast
                        }
                    }
*/
                    Column {
                        visible: lastupdate.text

                        Text {
                            text: qsTr("Last update")
                            textFormat: Text.PlainText
                            color: Theme.colorSubText
                            font.bold: true
                            font.pixelSize: Theme.fontSizeContentVerySmall
                            font.capitalization: Font.AllUppercase
                        }
                        Text {
                            id: lastupdate
                            text: currentDevice.lastUpdate.toLocaleString(Locale.ShortFormat)
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContentBig
                            color: Theme.colorHighContrast
                        }
                    }

                    Column {
                        visible: lastsync.text

                        Text {
                            text: qsTr("Last history sync")
                            textFormat: Text.PlainText
                            color: Theme.colorSubText
                            font.bold: true
                            font.pixelSize: Theme.fontSizeContentVerySmall
                            font.capitalization: Font.AllUppercase
                        }
                        Text {
                            id: lastsync
                            text: currentDevice.lastHistorySync.toLocaleString(Locale.ShortFormat)
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContentBig
                            color: Theme.colorHighContrast
                        }
                    }
                }
            }

            ////////////////////////////////

            Rectangle {
                id: itemDeviceInfos
                width: www
                height: itemDeviceInfosContent.height + 24

                radius: Theme.componentRadius
                color: Theme.colorForeground
                border.width: 2
                border.color: Theme.colorSeparator

                Column {
                    id: itemDeviceInfosContent
                    anchors.top: parent.top
                    anchors.topMargin: 12
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    spacing: 8

                    Column {
                        Text {
                            text: qsTr("Manufacturer")
                            textFormat: Text.PlainText
                            color: Theme.colorSubText
                            font.bold: true
                            font.pixelSize: Theme.fontSizeContentVerySmall
                            font.capitalization: Font.AllUppercase
                        }
                        Text {
                            text: currentDevice.deviceInfos.deviceManufacturer
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContentBig
                            color: Theme.colorHighContrast
                        }
                    }

                    Column {
                        Text {
                            text: qsTr("Product ID")
                            textFormat: Text.PlainText
                            color: Theme.colorSubText
                            font.bold: true
                            font.pixelSize: Theme.fontSizeContentVerySmall
                            font.capitalization: Font.AllUppercase
                        }
                        Text {
                            text: currentDevice.deviceInfos.deviceId
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContentBig
                            color: Theme.colorHighContrast
                        }
                    }

                    Row {
                        spacing: 32

                        Column {
                            Text {
                                text: qsTr("Year")
                                textFormat: Text.PlainText
                                color: Theme.colorSubText
                                font.bold: true
                                font.pixelSize: Theme.fontSizeContentVerySmall
                                font.capitalization: Font.AllUppercase
                            }
                            Text {
                                text: currentDevice.deviceInfos.deviceYear
                                textFormat: Text.PlainText
                                font.pixelSize: Theme.fontSizeContentBig
                                color: Theme.colorHighContrast
                            }
                        }

                        Column {
                            Text {
                                text: qsTr("IP Rating")
                                textFormat: Text.PlainText
                                color: Theme.colorSubText
                                font.bold: true
                                font.pixelSize: Theme.fontSizeContentVerySmall
                                font.capitalization: Font.AllUppercase
                            }
                            Text {
                                text: currentDevice.deviceInfos.deviceIPrating
                                textFormat: Text.PlainText
                                font.pixelSize: Theme.fontSizeContentBig
                                color: Theme.colorHighContrast
                            }
                        }
                    }

                    Row {
                        spacing: 32

                        Column {
                            visible: dbatt.text

                            Text {
                                text: qsTr("Battery")
                                textFormat: Text.PlainText
                                color: Theme.colorSubText
                                font.bold: true
                                font.pixelSize: Theme.fontSizeContentVerySmall
                                font.capitalization: Font.AllUppercase
                            }
                            Text {
                                id: dbatt
                                textFormat: Text.PlainText
                                text: currentDevice.deviceInfos.deviceBattery
                                font.pixelSize: Theme.fontSizeContentBig
                                color: Theme.colorHighContrast
                            }
                        }

                        Column {
                            visible: dscreen.text

                            Text {
                                text: qsTr("Screen")
                                textFormat: Text.PlainText
                                color: Theme.colorSubText
                                font.bold: true
                                font.pixelSize: Theme.fontSizeContentVerySmall
                                font.capitalization: Font.AllUppercase
                            }
                            Text {
                                id: dscreen
                                text: currentDevice.deviceInfos.deviceScreen
                                textFormat: Text.PlainText
                                font.pixelSize: Theme.fontSizeContentBig
                                color: Theme.colorHighContrast
                            }
                        }
                    }
                }
            }

            ////////////////////////////////

            Rectangle {
                id: itemDeviceSensors
                width: www
                height: itemDeviceSensorsContent.height + 24

                radius: Theme.componentRadius
                color: Theme.colorForeground
                border.width: 2
                border.color: Theme.colorSeparator

                Column {
                    id: itemDeviceSensorsContent
                    anchors.top: parent.top
                    anchors.topMargin: 12
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    spacing: 8

                    Text {
                        visible: repeaterSensors.count
                        text: qsTr("Sensors")
                        textFormat: Text.PlainText
                        color: Theme.colorSubText
                        font.bold: true
                        font.pixelSize: Theme.fontSizeContentVerySmall
                        font.capitalization: Font.AllUppercase
                    }

                    Repeater {
                        id: repeaterSensors
                        model: currentDevice.deviceInfos.deviceSensors

                        Row {
                            spacing: 12

                            RoundButtonIcon {
                                width: 40; height: 40;
                                borderVisible: true
                                backgroundVisible: true
                                backgroundColor: Theme.colorBackground
                                iconColor: Theme.colorText
                                source: UtilsDeviceSensors.getDeviceSensorIcon(modelData.sensorId)
                            }
                            Column {
                                anchors.verticalCenter: parent.verticalCenter
                                Text {
                                    text: UtilsDeviceSensors.getDeviceSensorName(modelData.sensorId)
                                    textFormat: Text.PlainText
                                    font.pixelSize: Theme.fontSizeContent
                                    color: Theme.colorText
                                }
                                Text {
                                    text: modelData.sensorString
                                    textFormat: Text.PlainText
                                    font.pixelSize: Theme.fontSizeContentSmall
                                    color: Theme.colorSubText
                                }
                            }
                        }
                    }

                    Text {
                        height: 24
                        verticalAlignment: Text.AlignBottom
                        visible: repeaterCapabilities.count

                        text: qsTr("Capabilities")
                        textFormat: Text.PlainText
                        color: Theme.colorSubText
                        font.bold: true
                        font.pixelSize: Theme.fontSizeContentVerySmall
                        font.capitalization: Font.AllUppercase
                    }

                    Repeater {
                        id: repeaterCapabilities
                        model: currentDevice.deviceInfos.deviceCapabilities

                        Row {
                            spacing: 12

                            RoundButtonIcon {
                                width: 40; height: 40;
                                borderVisible: true
                                backgroundVisible: true
                                backgroundColor: Theme.colorBackground
                                iconColor: Theme.colorText
                                source: UtilsDeviceSensors.getDeviceCapabilityIcon(modelData.capabilityId)
                            }
                            Column {
                                anchors.verticalCenter: parent.verticalCenter
                                Text {
                                    text: UtilsDeviceSensors.getDeviceCapabilityName(modelData.capabilityId)
                                    textFormat: Text.PlainText
                                    font.pixelSize: Theme.fontSizeContent
                                    color: Theme.colorText
                                }
                                Text {
                                    text: modelData.capabilityString
                                    textFormat: Text.PlainText
                                    font.pixelSize: Theme.fontSizeContentSmall
                                    color: Theme.colorSubText
                                }
                            }
                        }
                    }
                }
            }

            ////////////////////////////////
        }
    }
}
