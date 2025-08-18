import QtQuick
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

            Text {
                width: parent.width

                text: (popupGatewayConfirm.mode === "erase") ?
                          qsTr("Are you sure you want to erase this gateway settings?") :
                          qsTr("Are you sure you want to reboot this gateway? ")

                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeContentVeryBig
                color: Theme.colorText
                wrapMode: Text.WordWrap
            }

            Text {
                id: textErase
                width: parent.width

                text: (popupGatewayConfirm.mode === "erase") ?
                          qsTr("erase erase erase erase erase erase erase erase erase ") :
                          qsTr("reboot reboot reboot reboot reboot reboot reboot reboot ")
                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeContent
                color: Theme.colorSubText
                wrapMode: Text.WordWrap
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
