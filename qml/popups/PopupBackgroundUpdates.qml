import QtQuick
import QtQuick.Controls

import ComponentLibrary

Popup {
    id: popupBackgroundUpdates

    x: singleColumn ? 0 : (appWindow.width / 2) - (width / 2)
    y: singleColumn ? (appWindow.height - height)
                    : ((appWindow.height / 2) - (height / 2))

    width: singleColumn ? parent.width : 640
    height: contentColumn.height + padding*2 + screenPaddingNavbar + screenPaddingBottom
    padding: Theme.componentMarginXL

    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    parent: Overlay.overlay

    ////////////////////////////////////////////////////////////////////////////

    property bool locPerm: false

    onAboutToShow: {
         locPerm = utilsApp.checkMobileBackgroundLocationPermission()
    }

    ////////////////////////////////////////////////////////////////////////////

    background: Rectangle {
        color: Theme.colorBackground
        border.color: Theme.colorSeparator
        border.width: singleColumn ? 0 : Theme.componentBorderWidth
        radius: singleColumn ? 0 : Theme.componentRadius

        Rectangle {
            width: parent.width
            height: Theme.componentBorderWidth
            visible: singleColumn
            color: Theme.colorSeparator
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    contentItem: Item {
        Column {
            id: contentColumn
            width: parent.width
            spacing: Theme.componentMarginXL

            ////////

            Text {
                width: parent.width

                text: qsTr("About background updates")
                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeContentVeryBig
                color: Theme.colorText
                wrapMode: Text.WordWrap
            }

            ////////

            Column {
                width: parent.width
                spacing: Theme.componentMarginXL

                visible: !popupBackgroundUpdates.locPerm

                Column {
                    width: parent.width
                    spacing: 8

                    IconSvg {
                        width: 64
                        height: 64
                        anchors.horizontalCenter: parent.horizontalCenter

                        source: "qrc:/IconLibrary/material-icons/duotone/pin_drop.svg"
                        color: Theme.colorText
                    }

                    Text {
                        width: parent.width

                        text: qsTr("To scan for Bluetooth Low Energy sensors in the background, Android requires the <b>background location permission</b> — this is an OS-level requirement that applies to all BLE apps. Theengs only reads sensor advertisements; it does <b>not</b> track your location.<br><br>While scanning, an ongoing notification appears in your status bar so you always know Theengs is active. Tap <b>Stop</b> there to turn it off at any time.")
                        textFormat: Text.StyledText
                        font.pixelSize: Theme.fontSizeContent
                        color: Theme.colorSubText
                        wrapMode: Text.WordWrap
                    }
                }

                Flow {
                    width: parent.width
                    spacing: Theme.componentMargin

                    property var btnSize: singleColumn ? width : ((width-spacing*2) / 2)

                    ButtonSolid {
                        width: parent.btnSize

                        color: Theme.colorSecondary

                        text: qsTr("About Bluetooth permissions")
                        source: "qrc:/IconLibrary/material-symbols/info.svg"
                        sourceSize: 20

                        onClicked: {
                            if (utilsApp.getAndroidSdkVersion() >= 12)
                                Qt.openUrlExternally("https://developer.android.com/guide/topics/connectivity/bluetooth/permissions#declare-android12-or-higher")
                            else
                                Qt.openUrlExternally("https://developer.android.com/guide/topics/connectivity/bluetooth/permissions#declare-android11-or-lower")
                        }
                    }

                    ButtonSolid {
                        width: (parent.btnSize / 2 - 8)

                        color: Theme.colorGrey

                        text: qsTr("Cancel")
                        source: "qrc:/IconLibrary/material-symbols/close.svg"

                        onClicked: {
                            popupBackgroundUpdates.close()
                        }
                    }

                    ButtonSolid {
                        width: (parent.btnSize / 2 - 8)

                        color: Theme.colorGreen

                        text: qsTr("Enable")
                        source: "qrc:/IconLibrary/material-symbols/check.svg"

                        onClicked: {
                            utilsApp.getMobileBackgroundLocationPermission()
                            popupBackgroundUpdates.locPerm = true
                        }
                    }
                }
            }

            ////////

            Column {
                width: parent.width
                spacing: Theme.componentMarginXL

                visible: popupBackgroundUpdates.locPerm

                Column {
                    width: parent.width
                    spacing: 8

                    IconSvg {
                        width: 64
                        height: 64
                        anchors.horizontalCenter: parent.horizontalCenter

                        source: "qrc:/IconLibrary/material-icons/duotone/battery_alert.svg"
                        color: Theme.colorText
                    }

                    Text {
                        width: parent.width
                        text: qsTr("Some Android phones aggressively pause background apps to save battery, which can interrupt sensor scanning. For reliable updates, you may need to adjust a few settings from the Android <b>app info</b> screen:")
                        textFormat: Text.StyledText
                        font.pixelSize: Theme.fontSizeContent
                        color: Theme.colorSubText
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        width: parent.width
                        text: qsTr("- exclude Theengs from <b>battery optimisation</b>") + "<br>" +
                              qsTr("- on some devices (Xiaomi, OPPO, Huawei…), <b>autolaunch</b> must also be enabled")
                        textFormat: Text.StyledText
                        font.pixelSize: Theme.fontSizeContent
                        color: Theme.colorSubText
                        wrapMode: Text.WordWrap
                    }
                }

                Flow {
                    width: parent.width
                    spacing: Theme.componentMargin

                    property var btnSize: singleColumn ? width : ((width-spacing*2) / 2)

                    ButtonSolid {
                        width: parent.btnSize

                        color: Theme.colorSecondary

                        text: qsTr("About battery savers")
                        source: "qrc:/IconLibrary/material-symbols/info.svg"
                        sourceSize: 20

                        onClicked: {
                            Qt.openUrlExternally("https://dontkillmyapp.com/")
                        }
                    }

                    ButtonSolid {
                        width: parent.btnSize

                        color: Theme.colorPrimary

                        text: qsTr("Application info panel")
                        source: "qrc:/IconLibrary/material-icons/duotone/tune.svg"
                        sourceSize: 20

                        onClicked: {
                            utilsApp.openAndroidAppInfo("com.theengs.app")
                        }
                    }

                    ButtonSolid {
                        width: parent.btnSize

                        color: Theme.colorGreen
                        layoutDirection: Qt.RightToLeft

                        text: qsTr("I understand")
                        source: "qrc:/IconLibrary/material-symbols/check.svg"

                        onClicked: {
                            popupBackgroundUpdates.close()
                        }
                    }
                }
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
