import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "qrc:/js/UtilsNumber.js" as UtilsNumber
import ThemeEngine

Rectangle {
    id: temperatureRangeWidget

    implicitWidth: 256
    implicitHeight: 80

    height: columnContent.height + 8
    radius: Theme.componentRadius
    color: Theme.colorForeground

    ////////////////////////////////////////////////////////////////////////////

    Loader {
        id: popupDeleteLoader

        active: false
        asynchronous: false

        sourceComponent: PopupPresetRangeDelete {
            id: popupDelete
            onConfirmed: currentPreset.removeRange(modelData.name)
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Column {
        id: columnContent
        anchors.left: parent.left
        anchors.right: parent.right

        TextInputThemed { // range name
            anchors.left: parent.left
            anchors.right: parent.right
            height: 44

            readOnly: currentPreset.readOnly
            text: modelData.name

            onDisplayTextChanged: {
                if (currentPreset.isRangeNameValid(text)) {
                    modelData.name = text
                }
            }
            onEditingFinished: {
                if (currentPreset.isRangeNameValid(text)) {
                    modelData.name = text
                }
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 2
                z: -1
                color: Theme.colorSeparator
            }
        }

        Item {
            width: 12
            height: 6
        }

        RowLayout {
            anchors.left: parent.left
            anchors.leftMargin: Theme.componentMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.componentMargin
            height: 44

            Text {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                text: qsTr("Minimum")
                color: Theme.colorSubText
                font.pixelSize: Theme.componentFontSize
            }
            SpinBoxThemed {
                Layout.preferredHeight: 34
                Layout.alignment: Qt.AlignVCenter

                enabled: !currentPreset.readOnly
                editable: false
                legend: (settingsManager.appUnits === 0) ? "°C" : "°F"

                from: UtilsNumber.tempCelsiusOrFahrenheit(modelData.tempLimitMin_min, settingsManager.appUnits).toFixed(0)
                to: UtilsNumber.tempCelsiusOrFahrenheit(modelData.tempLimitMin_max, settingsManager.appUnits).toFixed(0)
                value: UtilsNumber.tempCelsiusOrFahrenheit(modelData.tempMin, settingsManager.appUnits)
                onValueModified: (settingsManager.appUnits === 0) ? modelData.tempMin = value :
                                                                    modelData.tempMin = UtilsNumber.tempFahrenheitToCelsius(value)
            }
        }

        RowLayout {
            anchors.left: parent.left
            anchors.leftMargin: Theme.componentMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.componentMargin
            height: 44

            Text {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                text: modelData.tempMaxEnabled ? qsTr("Maximum") : qsTr("No maximum")
                color: Theme.colorSubText
                font.pixelSize: Theme.componentFontSize
            }

            ButtonWireframe {
                Layout.preferredHeight: 34
                Layout.alignment: Qt.AlignVCenter

                visible: (!currentPreset.readOnly && modelData.tempMaxDisabled)
                text: qsTr("Set")

                onClicked: {
                    modelData.tempMaxDisabled = false
                    modelData.tempMax = modelData.tempMin + 4
                }
            }

            SpinBoxThemed {
                Layout.preferredHeight: 34
                Layout.alignment: Qt.AlignVCenter

                visible: modelData.tempMaxEnabled
                enabled: !currentPreset.readOnly
                editable: false
                legend: (settingsManager.appUnits === 0) ? "°C" : "°F"

                //textFromValue: function(value, locale) { return UtilsNumber.tempCelsiusOrFahrenheit(value, settingsManager.appUnits) }
                //valueFromText: function(text, locale) { return UtilsNumber.tempCelsiusOrFahrenheit(value, settingsManager.appUnits) }

                from: UtilsNumber.tempCelsiusOrFahrenheit(modelData.tempLimitMax_min, settingsManager.appUnits).toFixed(0)
                to: UtilsNumber.tempCelsiusOrFahrenheit(modelData.tempLimitMax_max, settingsManager.appUnits).toFixed(0)
                value: UtilsNumber.tempCelsiusOrFahrenheit(modelData.tempMax, settingsManager.appUnits)
                onValueModified: (settingsManager.appUnits === 0) ? modelData.tempMax = value :
                                                                    modelData.tempMax = UtilsNumber.tempFahrenheitToCelsius(value)
            }
        }

        Item {
            anchors.left: parent.left
            anchors.leftMargin: Theme.componentMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.componentMargin
            height: 44

            visible: !currentPreset.readOnly

            ButtonWireframeIcon {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                height: 34
                text: qsTr("Delete")

                onClicked: {
                    popupDeleteLoader.active = true
                    popupDeleteLoader.item.open()
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
