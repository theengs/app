import QtQuick
import QtQuick.Controls

import PresetUtils
import "qrc:/js/UtilsPresets.js" as UtilsPresets

import ThemeEngine

Popup {
    id: popupPresetAdd

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

    onAboutToShow: {
        presetName.focus = isDesktop
        presetName.text = ""
        presetType.currentSelection = 0
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

                text: qsTr("Create new temperature preset")
                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeContentVeryBig
                color: Theme.colorText
                wrapMode: Text.WordWrap
            }

            ////////

            Column {
                width: parent.width
                spacing: 8
/*
                Text {
                    width: parent.width

                    text: qsTr("Choose a name and a preset type.")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContent
                    color: Theme.colorSubText
                    wrapMode: Text.WordWrap
                }
*/
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

            SelectorGrid {
                id: presetType
                width: parent.width

                btnCols: 4
                btnRows: 2
                btnHeight: 52

                model: ListModel {
                    ListElement { idx:  0; txt: ""; src: "qrc:/assets/icons_fontawesome/question-solid.svg"; }
                    ListElement { idx:  1; txt: ""; src: "qrc:/assets/icons_fontawesome/cow-solid.svg"; }
                    ListElement { idx:  2; txt: ""; src: "qrc:/assets/icons_fontawesome/piggy-bank-solid.svg"; }
                    ListElement { idx:  3; txt: ""; src: "qrc:/assets/icons_fontawesome/kiwi-bird-solid.svg"; }
                    ListElement { idx:  4; txt: ""; src: "qrc:/assets/icons_fontawesome/fish-fins-solid.svg"; }
                }
                currentSelection: 0
                onMenuSelected: (index) => {
                    //console.log("SelectorMenu clicked #" + index)
                    currentSelection = index
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
                        popupPresetAdd.close()
                    }
                }

                ButtonWireframe {
                    width: parent.btnSize

                    text: qsTr("Create new preset")
                    primaryColor: Theme.colorPrimary
                    fullColor: true

                    enabled: presetsManager.isPresetNameValid(presetName.text)
                    onClicked: {
                        presetsManager.addPreset(presetType.currentSelection, presetName.text)
                        popupPresetAdd.close()
                    }
                }
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
