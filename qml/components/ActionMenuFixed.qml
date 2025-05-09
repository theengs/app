import QtQuick
import QtQuick.Controls

import ComponentLibrary

Popup {
    id: actionMenu
    width: 200

    padding: 0
    margins: 0

    parent: Overlay.overlay
    modal: true
    dim: false
    focus: isMobile
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    enter: Transition { NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 133; } }
    exit: Transition { NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 133; } }

    property int layoutDirection: Qt.RightToLeft

    signal menuSelected(var index)

    ////////////////////////////////////////////////////////////////////////////

    background: Rectangle {
        color: Theme.colorBackground
        radius: Theme.componentRadius
        border.color: Theme.colorSeparator
        border.width: Theme.componentBorderWidth
    }

    ////////////////////////////////////////////////////////////////////////////

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        topPadding: 8
        bottomPadding: 8
        spacing: 4

        ////////

        ActionMenuItem_button {
            id: actionUpdate

            index: 0
            text: qsTr("Update data")
            source: "qrc:/IconLibrary/material-symbols/refresh.svg"
            layoutDirection: actionMenu.layoutDirection
            visible: (deviceManager.bluetooth && (selectedDevice && selectedDevice.hasBluetoothConnection))

            onClicked: {
                deviceRefreshButtonClicked()
                menuSelected(index)
                close()
            }
        }

        ActionMenuItem_button {
            id: actionRealtime

            index: 1
            text: qsTr("Real time data")
            source: "qrc:/IconLibrary/material-icons/duotone/update.svg"
            layoutDirection: actionMenu.layoutDirection
            visible: (deviceManager.bluetooth && (selectedDevice && selectedDevice.hasRealTime))

            onClicked: {
                deviceRefreshRealtimeButtonClicked()
                menuSelected(index)
                close()
            }
        }

        ////////

        Rectangle {
            width: parent.width; height: 1;
            color: Theme.colorSeparator
            visible: (deviceManager.bluetooth && (selectedDevice && selectedDevice.hasHistory))
        }

        ActionMenuItem_button {
            id: actionHistoryRefresh

            index: 2
            text: qsTr("Update history")
            source: "qrc:/IconLibrary/material-icons/duotone/date_range.svg"
            layoutDirection: actionMenu.layoutDirection
            visible: (deviceManager.bluetooth && (selectedDevice && selectedDevice.hasHistory))

            onClicked: {
                deviceRefreshHistoryButtonClicked()
                menuSelected(index)
                close()
            }
        }

        ActionMenuItem_button {
            id: actionHistoryClear

            index: 3
            text: qsTr("Clear history")
            source: "qrc:/IconLibrary/material-icons/duotone/date_clear.svg"
            layoutDirection: actionMenu.layoutDirection
            visible: (deviceManager.bluetooth && (selectedDevice && selectedDevice.hasHistory))

            onClicked: {
                deviceClearButtonClicked()
                menuSelected(index)
                close()
            }
        }

        ////////

        Rectangle {
            width: parent.width; height: 1;
            color: Theme.colorSeparator
            visible: (actionUpdate.visible || actionHistoryRefresh.visible) && (actionLed.visible || actionWatering.visible || actionGraphMode.visible)
        }

        ActionMenuItem_button {
            id: actionLed

            index: 5
            text: qsTr("Blink LED")
            source: "qrc:/IconLibrary/material-icons/duotone/emoji_objects.svg"
            layoutDirection: actionMenu.layoutDirection
            visible: (deviceManager.bluetooth && (selectedDevice && selectedDevice.hasLED))

            onClicked: {
                deviceLedButtonClicked()
                menuSelected(index)
                close()
            }
        }

        ActionMenuItem_button {
            id: actionWatering

            index: 6
            text: qsTr("Watering")
            source: "qrc:/IconLibrary/material-icons/duotone/local_drink.svg"
            layoutDirection: actionMenu.layoutDirection
            visible: (deviceManager.bluetooth && (selectedDevice && selectedDevice.hasWaterTank))

            onClicked: {
                deviceWateringButtonClicked()
                menuSelected(index)
                close()
            }
        }

        ActionMenuItem_button {
            id: actionGraphMode

            index: 7
            text: qsTr("Switch graph")
            source: (settingsManager.graphThermometer === "minmax") ? "qrc:/IconLibrary/material-icons/duotone/insert_chart.svg" : "qrc:/IconLibrary/material-symbols/timeline.svg"
            layoutDirection: actionMenu.layoutDirection
            visible: (appContent.state === "DeviceThermometer")

            onClicked: {
                if (settingsManager.graphThermometer === "minmax") settingsManager.graphThermometer = "lines"
                else settingsManager.graphThermometer = "minmax"
                menuSelected(index)
                close()
            }
        }

        ActionMenuItem_button {
            id: actionCalibrate

            index: 8
            text: qsTr("Calibrate sensor")
            source: "qrc:/IconLibrary/material-icons/duotone/model_training.svg"
            layoutDirection: actionMenu.layoutDirection
            visible: (deviceManager.bluetooth && (selectedDevice && selectedDevice.hasCalibration))

            onClicked: {
                deviceCalibrateButtonClicked()
                menuSelected(index)
                close()
            }
        }

        ActionMenuItem_button {
            id: actionMacAddress

            index: 9
            text: qsTr("Set MAC address")
            source: "qrc:/IconLibrary/material-symbols/pin.svg"
            layoutDirection: actionMenu.layoutDirection
            visible: (selectedDevice && (Qt.platform.os === "osx" || Qt.platform.os === "ios"))

            onClicked: {
                deviceMacButtonClicked()
                menuSelected(index)
                close()
            }
        }
    }
}
