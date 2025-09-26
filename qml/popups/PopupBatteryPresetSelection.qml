import QtQuick
import QtQuick.Controls

import ComponentLibrary

Popup {
    id: popupBatteryPresets

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

    onAboutToShow: {
        batteryPresetsManager.filter(presetSearchBox.displayText)
    }

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
            anchors.margins: Theme.componentRadius

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

                font.pixelSize: 18
                font.bold: false
                color: Theme.colorText

                placeholderText: qsTr("Filter presets")

                onDisplayTextChanged: batteryPresetsManager.filter(presetSearchBox.displayText)

                Row {
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: Theme.componentMargin

                    Text {
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("%1 presets").arg(((presetSearchBox.displayText) ? batteryPresetsManager.presetCountFiltered : batteryPresetsManager.presetCount))
                        font.pixelSize: Theme.fontSizeContentSmall
                        color: Theme.colorSubText
                    }

                    RoundButtonIcon {
                        width: 24
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter

                        visible: presetSearchBox.text.length
                        highlightMode: "color"
                        source: "qrc:/IconLibrary/material-symbols/backspace-fill.svg"

                        onClicked: presetSearchBox.text = ""
                    }

                    IconSvg {
                        width: 24
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter

                        source: "qrc:/IconLibrary/material-symbols/search.svg"
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

            model: batteryPresetsManager.presetsFiltered
            delegate: BatteryPresetSearchWidget {
                width: ListView.view.width - Theme.componentBorderWidth*2
                height: 64

                selected: (currentPreset && currentPreset.name === modelData.name)

                onClicked: {
                    popupBatteryPresets.selected(modelData.name)
                    popupBatteryPresets.close()
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

            ButtonSolid {
                width: parent.btnSize

                text: qsTr("Cancel")
                color: Theme.colorGrey

                onClicked: popupBatteryPresets.close()
            }

            ButtonSolid {
                width: parent.btnSize

                text: qsTr("Don't use a preset")
                color: Theme.colorSubText

                onClicked: {
                    popupBatteryPresets.selected(null)
                    popupBatteryPresets.close()
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
