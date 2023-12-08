import QtQuick
import QtQuick.Controls

import ThemeEngine

Popup {
    id: popupPresetSelection

    x: singleColumn ? 0 : (appWindow.width / 2) - (width / 2)
    y: singleColumn ? 0 : ((appWindow.height / 2) - (height / 2))

    width: singleColumn ? parent.width : 640
    height: singleColumn ? parent.height : 800
    padding: 0

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

        Rectangle { // OS statusbar area
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            height: Math.max(screenPaddingStatusbar, screenPaddingTop)
            color: Theme.colorStatusbar
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    contentItem: Item {
        anchors.top: parent.top
        anchors.topMargin: Math.max(screenPaddingStatusbar, screenPaddingTop)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Math.max(screenPaddingNavbar, screenPaddingBottom)

        Rectangle { // search area
            id: presetSearchArea
            anchors.left: parent.left
            anchors.right: parent.right

            height: presetSearchBox.height + Theme.componentMarginXL*2
            color: Theme.colorBackground
            z: 4

            TextFieldThemed {
                id: presetSearchBox
                anchors.top: parent.top
                anchors.topMargin: Theme.componentMarginXL
                anchors.left: parent.left
                anchors.leftMargin: Theme.componentMarginXL
                anchors.right: parent.right
                anchors.rightMargin: Theme.componentMarginXL

                width: parent.width
                height: 48
                z: 2

                font.pixelSize: 20
                font.bold: false
                color: Theme.colorText

                placeholderText: qsTr("Filter presets")

                onDisplayTextChanged: presetsManager.filter(displayText)

                Row {
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: Theme.componentMargin

                    Text {
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("%1 presets").arg(((presetSearchBox.displayText) ? presetsManager.presetCountFiltered : presetsManager.presetCount))
                        font.pixelSize: Theme.fontSizeContentSmall
                        color: Theme.colorSubText
                    }

                    RoundButtonIcon {
                        width: 24
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter

                        visible: presetSearchBox.text.length
                        highlightMode: "color"
                        source: "qrc:/assets/icons_material/baseline-backspace-24px.svg"

                        onClicked: presetSearchBox.text = ""
                    }

                    IconSvg {
                        width: 24
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter

                        source: "qrc:/assets/icons_material/baseline-search-24px.svg"
                        color: Theme.colorText
                    }
                }
            }
        }

        ListView {
            anchors.top: presetSearchArea.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: flowButtons.top

            topMargin: 0
            bottomMargin: 0
            spacing: 0
            clip: true

            model: presetsManager.presetsFiltered
            delegate: TemperaturePresetSearchWidget {
                width: ListView.view.width
                height: 72

                onClicked: {
                    popupPresetSelection.selected(modelData.name)
                    popupPresetSelection.close()
                }
            }
        }

        Flow {
            id: flowButtons
            anchors.left: parent.left
            anchors.leftMargin: Theme.componentMarginXL
            anchors.right: parent.right
            anchors.rightMargin: Theme.componentMarginXL
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Theme.componentMarginXL

            width: parent.width
            z: 4
            spacing: Theme.componentMargin
            property var btnSize: singleColumn ? width : ((width-spacing) / 2)

            ButtonWireframe {
                width: parent.btnSize

                text: qsTr("Cancel")
                primaryColor: Theme.colorSubText
                secondaryColor: Theme.colorForeground

                onClicked: popupPresetSelection.close()
            }

            ButtonWireframe {
                width: parent.btnSize

                text: qsTr("Don't use a preset")
                primaryColor: Theme.colorSubText
                secondaryColor: Theme.colorForeground

                onClicked: {
                    popupPresetSelection.selected(null)
                    popupPresetSelection.close()
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
