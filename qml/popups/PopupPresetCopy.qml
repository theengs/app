import QtQuick
import QtQuick.Controls

import PresetUtils
import "qrc:/js/UtilsPresets.js" as UtilsPresets

import ThemeEngine

Popup {
    id: popupPresetCopy

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

    onAboutToShow: {
        presetName.focus = isDesktop
        presetName.text = ""
    }
    onAboutToHide: {
        //
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

                text: qsTr("Copy the current preset")
                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeContentVeryBig
                color: Theme.colorText
                wrapMode: Text.WordWrap
            }

            ////////

            Column {
                width: parent.width
                spacing: 8

                Text {
                    width: parent.width

                    text: qsTr("The new preset will become editable.")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContent
                    color: Theme.colorSubText
                    wrapMode: Text.WordWrap
                }

                TextFieldThemed {
                    id: presetName
                    width: parent.width
                    height: 48

                    font.pixelSize: 20
                    font.bold: false
                    color: Theme.colorText

                    placeholderText: "Preset name"
                }

                Row {
                    width: parent.width
                    height: 44
                    leftPadding: Theme.componentMargin/2
                    spacing: Theme.componentMargin/2

                    visible: presetName.length && !presetsManager.isPresetNameValid(presetName.text)

                    IconSvg {
                        width: 24; height: 24;
                        anchors.verticalCenter: parent.verticalCenter
                        source: "qrc:/assets/icons_material/baseline-warning-24px.svg"
                        color: Theme.colorWarning
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("Invalid name.")
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.fontSizeContent
                        color: Theme.colorSubText
                        wrapMode: Text.WordWrap
                    }
                }
            }

            ////////

            Flow {
                width: parent.width
                spacing: Theme.componentMargin

                property var btnSize: singleColumn ? width : ((width-spacing) / 2)

                ButtonWireframe {
                    width: parent.btnSize

                    text: qsTr("Cancel")
                    primaryColor: Theme.colorSubText
                    secondaryColor: Theme.colorForeground

                    onClicked: {
                        popupPresetCopy.close()
                    }
                }

                ButtonWireframe {
                    width: parent.btnSize

                    text: qsTr("Copy preset")
                    primaryColor: Theme.colorPrimary
                    fullColor: true

                    enabled: presetsManager.isPresetNameValid(presetName.text)
                    onClicked: {
                        presetsManager.copyPreset(currentPreset.name, presetName.text)
                        popupPresetCopy.confirmed()
                        popupPresetCopy.close()
                    }
                }
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
