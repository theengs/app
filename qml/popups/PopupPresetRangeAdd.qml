import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import PresetUtils
import "qrc:/js/UtilsPresets.js" as UtilsPresets
import "qrc:/js/UtilsNumber.js" as UtilsNumber

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
                    textFormat: Text.PlainText
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
                        textFormat: Text.PlainText
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
                            if (currentPreset.rangeCount === 0) { // first
                                spinboxMin.from = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMin(), settingsManager.appUnits)
                                spinboxMin.to = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMax(), settingsManager.appUnits)
                                spinboxMin.value = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMin_default(), settingsManager.appUnits)

                                spinboxMax.from = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMin(), settingsManager.appUnits)
                                spinboxMax.to = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMax(), settingsManager.appUnits)
                                spinboxMax.value = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMax_default(), settingsManager.appUnits)
                            } else if (currentSelection === 0) { // before
                                spinboxMin.from = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMin(), settingsManager.appUnits)
                                spinboxMin.to = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMin_add(), settingsManager.appUnits)
                                spinboxMax.from = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMin(), settingsManager.appUnits)
                                spinboxMax.to = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMin_add(), settingsManager.appUnits)

                                spinboxMin.value = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMin_add() - 6, settingsManager.appUnits)
                                spinboxMax.value = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMin_add() - 3, settingsManager.appUnits)
                            } else { // after
                                spinboxMin.from = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMax_add(), settingsManager.appUnits)
                                spinboxMin.to = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMax(), settingsManager.appUnits)
                                spinboxMax.from = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMax_add(), settingsManager.appUnits)
                                spinboxMax.to = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMax(), settingsManager.appUnits)

                                spinboxMin.value = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMax_add() + 3, settingsManager.appUnits)
                                spinboxMax.value = UtilsNumber.tempCelsiusOrFahrenheit(currentPreset.getTempMax_add() + 6, settingsManager.appUnits)
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

                        editable: false
                        legend: (settingsManager.appUnits == 0) ? "°C" : "°F"
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
                        editable: false
                        legend: (settingsManager.appUnits == 0) ? "°C" : "°F"
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
                        var valuemin = (settingsManager.appUnits === 0) ? spinboxMin.value :
                                         UtilsNumber.tempFahrenheitToCelsius(spinboxMin.value)
                        var valuemax = (settingsManager.appUnits === 0) ? spinboxMax.value :
                                         UtilsNumber.tempFahrenheitToCelsius(spinboxMax.value)

                        currentPreset.addRange(rangeName.text, beforeAfterToogle.value,
                                               valuemin, valuemax, checkboxMax.checked)
                        popupPresetRangeAdd.close()
                    }
                }
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
