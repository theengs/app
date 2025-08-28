import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import ComponentLibrary

Popup {
    id: popupGatewayConfirm

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

    signal confirmed()

    property string mode: "erase"

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

            Text { // title
                width: parent.width

                text: (popupGatewayConfirm.mode === "erase") ?
                          qsTr("Are you sure you want to erase this gateway settings?") :
                          qsTr("Are you sure you want to reboot this gateway? ")

                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeContentVeryBig
                color: Theme.colorText
                wrapMode: Text.WordWrap
            }

            Column {
                width: parent.width
                spacing: Theme.componentMargin

                Text { // textErase1
                    width: parent.width

                    visible: (popupGatewayConfirm.mode === "erase")

                    text: qsTr("erase erase erase erase erase erase erase erase erase")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContent
                    color: Theme.colorSubText
                    wrapMode: Text.WordWrap
                }

                RowLayout { // textErase2
                    width: parent.width
                    spacing: Theme.componentMargin

                    visible: (popupGatewayConfirm.mode === "erase")

                    IconSvg {
                        Layout.preferredWidth: 20
                        Layout.preferredHeight: 20
                        Layout.alignment: Qt.AlignVCenter

                        source: "qrc:/IconLibrary/material-symbols/warning.svg"
                        color: Theme.colorWarning
                    }
                    Text {
                        Layout.fillWidth: singleColumn
                        Layout.alignment: Qt.AlignVCenter

                        text: qsTr("This will <b>erase</b> every settings.")
                        textFormat: Text.StyledText
                        font.pixelSize: Theme.fontSizeContent
                        color: Theme.colorSubText
                        wrapMode: Text.WordWrap
                        horizontalAlignment: singleColumn ? Text.AlignJustify : Text.AlignHCenter
                    }
                }

                Text { // textReboot
                    width: parent.width

                    visible: (popupGatewayConfirm.mode === "reboot")

                    text: qsTr("reboot reboot reboot reboot reboot reboot reboot reboot")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContent
                    color: Theme.colorSubText
                    wrapMode: Text.WordWrap
                }
            }

            ////////

            Flow {
                width: parent.width
                spacing: Theme.componentMargin

                property var btnSize: singleColumn ? width : ((width-spacing) / 2)

                ButtonSolid {
                    width: parent.btnSize

                    text: qsTr("Cancel")
                    color: Theme.colorGrey

                    onClicked: popupGatewayConfirm.close()
                }

                ButtonSolid {
                    width: parent.btnSize

                    text: (popupGatewayConfirm.mode === "reboot") ? qsTr("Reboot") : qsTr("Erase")
                    color: Theme.colorPrimary

                    onClicked: {
                        if (popupGatewayConfirm.mode === "erase") {
                            currentGateway.actionErase()
                        } else if (popupGatewayConfirm.mode === "reboot") {
                            currentGateway.actionRestart()
                        }

                        popupGatewayConfirm.confirmed()
                        popupGatewayConfirm.close()
                    }
                }
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
