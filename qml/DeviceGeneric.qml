import QtQuick 2.15
import QtQuick.Controls 2.15

import ThemeEngine 1.0
import DeviceUtils 1.0
import "qrc:/js/UtilsDeviceSensors.js" as UtilsDeviceSensors

Loader {
    id: deviceGeneric

    property var currentDevice: null

    ////////

    function loadDevice(clickedDevice) {
        // set device
        if (typeof clickedDevice === "undefined" || !clickedDevice) return
        if (!clickedDevice.isGenericDevice) return
        if (clickedDevice === currentDevice) return
        currentDevice = clickedDevice

        // load screen
        deviceGeneric.active = true
        deviceGeneric.item.loadDevice()
    }

    ////////

    function backAction() {
        if (deviceGeneric.status === Loader.Ready)
            deviceGeneric.item.backAction()
    }

    ////////////////////////////////////////////////////////////////////////////

    active: false
    asynchronous: false

    sourceComponent: Item {
        id: itemDeviceGeneric
        implicitWidth: 480
        implicitHeight: 720

        focus: parent.focus

        property string cccc: headerUnicolor ? Theme.colorHeaderContent : "white"

        ////////

        Connections {
            target: currentDevice
            function onSensorUpdated() { updateHeader() }
            function onSensorsUpdated() { updateHeader() }
            function onCapabilitiesUpdated() { updateHeader() }
            function onStatusUpdated() { updateHeader() }
            function onDataUpdated() {
                //
            }
            function onRefreshUpdated() {
                //
            }
            function onHistoryUpdated() {
                //
            }
        }

        Connections {
            target: appHeader
            // desktop only
            function onDeviceDataButtonClicked() {
                appHeader.setActiveDeviceData()
            }
            function onDeviceSettingsButtonClicked() {
                appHeader.setActiveDeviceSettings()
            }
            // mobile only
            function onRightMenuClicked() {
                //
            }
        }

        Timer {
            interval: 60000; running: true; repeat: true;
            onTriggered: updateStatusText()
        }

        Keys.onPressed: (event) => {
            if (event.key === Qt.Key_F5) {
                event.accepted = true
                deviceManager.updateDevice(currentDevice.deviceAddress)
            } else if (event.key === Qt.Key_Backspace) {
                event.accepted = true
                appWindow.backAction()
            }
        }

        ////////

        function loadDevice() {
            //console.log("DeviceGeneric // loadDevice() >> " + currentDevice)

            updateHeader()
            updateData()
        }

        function updateHeader() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isGenericDevice) return
            //console.log("DeviceGeneric // updateHeader() >> " + currentDevice)

            // Status
            updateStatusText()
        }

        function updateData() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isGenericDevice) return
            //console.log("DeviceGeneric // updateData() >> " + currentDevice)
        }

        function updateStatusText() {
            if (typeof currentDevice === "undefined" || !currentDevice) return
            if (!currentDevice.isGenericDevice) return
            //console.log("DeviceGeneric // updateStatusText() >> " + currentDevice)

            // Status
            textStatus.text = UtilsDeviceSensors.getDeviceStatusText(currentDevice.status)

            if (currentDevice.status === DeviceUtils.DEVICE_OFFLINE &&
                (currentDevice.isDataFresh_rt() || currentDevice.isDataToday())) {
                if (currentDevice.lastUpdateMin <= 1)
                    textStatus.text = qsTr("Synced")
                else
                    textStatus.text = qsTr("Synced %1 ago").arg(currentDevice.lastUpdateStr)
            }
        }

        ////////

        function backAction() {
            if (textInputLocation.focus) {
                textInputLocation.focus = false
                return
            }
            if (isHistoryMode()) {
                resetHistoryMode()
                return
            }

            appContent.state = "DeviceList"
        }

        function isHistoryMode() {
            return false
        }
        function resetHistoryMode() {
            //
        }

        ////////////////////////////////////////////////////////////////////////

        Flow {
            anchors.fill: parent

            Rectangle {
                id: headerBox

                property int dimboxw: Math.min(deviceGeneric.width * 0.4, isPhone ? 256 : 512)
                property int dimboxh: Math.max(deviceGeneric.height * 0.2, isPhone ? 128 : 256)

                width: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return parent.width
                    return singleColumn ? parent.width : dimboxw
                }
                height: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return dimboxh
                    return singleColumn ? dimboxh : parent.height
                }

                color: Theme.colorHeader
                z: 5

                ////////

                Column {
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 12

                    ////////

                    Text {
                        visible: isDesktop
                        text: currentDevice.deviceName
                        color: cccc
                        font.bold: false
                        font.pixelSize: 17
                        elide: Text.ElideRight
                    }

                    ////////

                    Row {
                        id: itemLocation
                        height: 24
                        spacing: 4

                        IconSvg {
                            id: imageLocation
                            width: 24
                            height: 24
                            anchors.verticalCenter: parent.verticalCenter

                            source: "qrc:/assets/icons_material/duotone-pin_drop-24px.svg"
                            color: cccc
                        }
                        TextInput {
                            id: textInputLocation
                            anchors.verticalCenter: parent.verticalCenter

                            padding: 4
                            font.pixelSize: 17
                            font.bold: false
                            color: cccc

                            text: currentDevice.deviceLocationName
                            onEditingFinished: {
                                currentDevice.deviceLocationName = text
                                focus = false
                            }

                            MouseArea {
                                id: textInputLocationArea
                                anchors.fill: parent
                                anchors.topMargin: -4
                                anchors.leftMargin: -24
                                anchors.rightMargin: -4
                                anchors.bottomMargin: -4

                                hoverEnabled: true
                                propagateComposedEvents: true

                                onPressed: (mouse) => {
                                    textInputLocation.forceActiveFocus()
                                    mouse.accepted = false
                                }
                            }
                        }
                        IconSvg {
                            anchors.verticalCenter: parent.verticalCenter
                            width: 20; height: 20;

                            source: "qrc:/assets/icons_material/duotone-edit-24px.svg"
                            color: cccc

                            opacity: (isMobile || !textInputLocation.text || textInputLocation.focus || textInputLocationArea.containsMouse) ? 0.9 : 0
                            Behavior on opacity { OpacityAnimator { duration: 133 } }
                        }
                    }

                    ////////

                    Row {
                        id: status
                        height: 24
                        spacing: 8

                        IconSvg {
                            id: imageStatus
                            width: 24
                            height: 24
                            anchors.verticalCenter: parent.verticalCenter

                            source: "qrc:/assets/icons_material/duotone-schedule-24px.svg"
                            color: cccc
                        }
                        Text {
                            id: textStatus
                            anchors.verticalCenter: parent.verticalCenter

                            text: qsTr("Loading…")
                            color: cccc
                            font.bold: false
                            font.pixelSize: 17
                            elide: Text.ElideRight
                        }
                    }

                    ////////
                }
            }

            ////////////////////////////////

            Item {
                id: sensorBox
                width: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return parent.width
                    return singleColumn ? parent.width : (parent.width - headerBox.width)
                }
                height: {
                    if (isTablet && screenOrientation == Qt.PortraitOrientation) return (parent.height - headerBox.height)
                    return singleColumn ? (parent.height - headerBox.height) : parent.height
                }

                ListView {
                    id: genericDataView
                    anchors.fill: parent
                    anchors.margins: 0

                    model: currentDevice.genericData
                    delegate: Rectangle {
                        width: genericDataView.width
                        height: 72
                        color: Theme.colorForeground

                        Column {
                            anchors.left: parent.left
                            anchors.leftMargin: 12
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 2

                            Text {
                                text: modelData.name
                                textFormat: Text.PlainText
                                color: Theme.colorText
                                font.pixelSize: Theme.fontSizeContentBig
                                font.capitalization: Font.Capitalize
                            }
                            Row {
                                spacing: 4
                                Text {
                                    text: {
                                        if (modelData.value_f > -99) return modelData.value_f.toFixed(2)
                                        return modelData.value
                                    }
                                    textFormat: Text.PlainText
                                    color: Theme.colorText
                                    font.pixelSize: Theme.fontSizeContentBig
                                }
                                Text {
                                    text: modelData.unit
                                    textFormat: Text.PlainText
                                    color: Theme.colorSubText
                                    font.pixelSize: Theme.fontSizeContent
                                }
                            }
                        }
                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            height: 1
                            color: Theme.colorSeparator
                        }
                    }
                }
            }

            ////////////////////////////////
        }

        ////////////////////////////////////////////////////////////////////////
    }
}
