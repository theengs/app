import QtQuick
import QtQuick.Controls

import ThemeEngine

Popup {
    id: popupPresetRangeDelete

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

            Text {
                width: parent.width

                text: qsTr("Are you sure you want to delete the preset selected?")
                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeContentVeryBig
                color: Theme.colorText
                wrapMode: Text.WordWrap
            }

            Text {
                width: parent.width

                text: qsTr("You can always add more presets later.", "")
                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeContent
                color: Theme.colorSubText
                wrapMode: Text.WordWrap
            }

            Flow {
                width: parent.width
                spacing: Theme.componentMargin

                property var btnSize: singleColumn ? width : ((width-spacing) / 2)

                ButtonWireframe {
                    width: parent.btnSize

                    text: qsTr("Cancel")
                    primaryColor: Theme.colorSubText
                    secondaryColor: Theme.colorForeground

                    onClicked: popupPresetRangeDelete.close()
                }

                ButtonWireframe {
                    width: parent.btnSize

                    text: qsTr("Delete")
                    primaryColor: Theme.colorRed
                    fullColor: true

                    onClicked: {
                        popupPresetRangeDelete.confirmed()
                        popupPresetRangeDelete.close()
                    }
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
