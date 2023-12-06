import QtQuick
import QtQuick.Controls

import ThemeEngine

Popup {
    id: popupPresetSelection

    x: singleColumn ? 0 : (appWindow.width / 2) - (width / 2)
    y: singleColumn ? 0 : ((appWindow.height / 2) - (height / 2))

    width: singleColumn ? parent.width : 640
    height: singleColumn ? parent.height : 800 // contentColumn.height + padding*2 + screenPaddingNavbar + screenPaddingBottom
    padding: Theme.componentMarginXL

    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    parent: Overlay.overlay

    signal selected(var name)

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

        TextFieldThemed {
            id: presetFilter
            anchors.top: parent.top
            width: parent.width
            height: 48

            font.pixelSize: 20
            font.bold: false
            color: Theme.colorText

            placeholderText: "Filter preset name"

            onDisplayTextChanged: {
                presetsManager.filter(displayText)
            }
        }

        ListView {
            anchors.fill: parent
            anchors.topMargin: 64
            anchors.bottomMargin: 64

            topMargin: Theme.componentMargin
            bottomMargin: Theme.componentMargin
            spacing: 0

            model: presetsManager.presetsFiltered
            delegate: TemperaturePresetWidget {
                width: ListView.view.width
                height: 92

                onClicked: {
                    popupPresetSelection.selected(modelData.name)
                    popupPresetSelection.close()
                }
            }
        }

        Flow {
            anchors.bottom: parent.bottom
            width: parent.width
            spacing: Theme.componentMargin

            property var btnSize: singleColumn ? width : ((width-spacing) / 2)

            ButtonWireframe {
                width: parent.btnSize

                text: qsTr("Cancel")
                primaryColor: Theme.colorSubText
                secondaryColor: Theme.colorForeground

                onClicked: popupPresetSelection.close()
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
