import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import PresetUtils
import "qrc:/js/UtilsPresets.js" as UtilsPresets

import ThemeEngine

Popup {
    id: popupPresetRangeAdd

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
        rangeName.focus = isDesktop
        rangeName.text = ""

        beforeAfterToogle.currentSelection = 1
        beforeAfterToogle.setMinMax()
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

                text: qsTr("Add a new temperature range")
                textFormat: Text.PlainText
                font.pixelSize: Theme.fontSizeContentVeryBig
                color: Theme.colorText
                wrapMode: Text.WordWrap
            }

            ////////

            Column {
                width: parent.width
                spacing: 12
/*
                Text {
                    width: parent.width

                    text: qsTr("Choose a name and a preset type.")
                    textFormat: Text.StyledText
                    font.pixelSize: Theme.fontSizeContent
                    color: Theme.colorSubText
                    wrapMode: Text.WordWrap
                }
*/
                TextFieldThemed {
                    id: rangeName
                    width: parent.width
                    height: 48

                    font.pixelSize: 20
                    font.bold: false
                    color: Theme.colorText

                    placeholderText: qsTr("Range name")
                }

                Row {
                    width: parent.width
                    height: 44
                    leftPadding: Theme.componentMargin/2
                    spacing: Theme.componentMargin/2

                    visible: rangeName.length && !currentPreset.isRangeNameValid(rangeName.text)

                    IconSvg {
                        width: 24; height: 24;
                        anchors.verticalCenter: parent.verticalCenter
                        source: "qrc:/assets/icons_material/baseline-warning-24px.svg"
                        color: Theme.colorWarning
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("Invalid name.")
                        textFormat: Text.StyledText
                        font.pixelSize: Theme.fontSizeContent
                        color: Theme.colorSubText
                        wrapMode: Text.WordWrap
                    }
                }

                Item {
                    width: parent.width
                    height: 44
                    visible: currentPreset && currentPreset.rangeCount

                    SelectorMenu {
                        id: beforeAfterToogle
                        anchors.centerIn: parent
                        height: 36

                        property bool value: (currentSelection === 0) ? true : false
                        model: ListModel {
                            ListElement { idx: 0; txt: qsTr("add before"); src: ""; sz: 16; }
                            ListElement { idx: 1; txt: qsTr("add after"); src: ""; sz: 16; }
                        }

                        currentSelection: 1
                        onMenuSelected: (index) => {
                            currentSelection = index
                            setMinMax()
                        }

                        function setMinMax() {
                            if (currentSelection === 0) { // before
                                spinboxMin.value = currentPreset.getTempMin_add() - 6
                                spinboxMax.value = currentPreset.getTempMin_add() - 2

                                spinboxMin.from = 0
                                spinboxMin.to = currentPreset.getTempMin_add() - 2
                                spinboxMax.from = 0
                                spinboxMax.to = currentPreset.getTempMin_add()
                            } else { // after
                                spinboxMin.value = currentPreset.getTempMax_add() + 2
                                spinboxMax.value = currentPreset.getTempMax_add() + 6

                                spinboxMin.from = currentPreset.getTempMax_add()
                                spinboxMin.to = 200
                                spinboxMax.from = currentPreset.getTempMax_add() + 2
                                spinboxMax.to = 200
                            }
                        }
                    }
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 36

                    CheckBoxThemed {
                        checked: true
                        checkable: false
                    }

                    Text {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter

                        text: qsTr("Minimum")
                        color: Theme.colorSubText
                    }

                    SpinBoxThemed {
                        id: spinboxMin
                        Layout.alignment: Qt.AlignVCenter
                        Layout.preferredHeight: 36

                        from: 0
                        to: 200
                    }
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 36

                    CheckBoxThemed {
                        id: checkboxMax
                        Layout.alignment: Qt.AlignVCenter

                        checked: true
                    }

                    Text {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter

                        text: checkboxMax.checked ? qsTr("Maximum") : qsTr("No maximum")
                        color: Theme.colorSubText
                    }

                    SpinBoxThemed {
                        id: spinboxMax
                        Layout.alignment: Qt.AlignVCenter
                        Layout.preferredHeight: 36

                        visible: checkboxMax.checked

                        from: 0
                        to: 200
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
                        popupPresetRangeAdd.close()
                    }
                }

                ButtonWireframe {
                    width: parent.btnSize

                    text: qsTr("Add range")
                    primaryColor: Theme.colorPrimary
                    fullColor: true

                    enabled: currentPreset && currentPreset.isRangeNameValid(rangeName.text)
                    onClicked: {
                        currentPreset.addRange(rangeName.text, beforeAfterToogle.value,
                                               spinboxMin.value, spinboxMax.value, checkboxMax.checked)
                        popupPresetRangeAdd.close()
                    }
                }
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
