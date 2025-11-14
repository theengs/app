import QtQuick
import QtQuick.Controls

import ComponentLibrary

Item {
    id: screenGatewayList
    anchors.fill: parent

    ////////////////////////////////////////////////////////////////////////////

    Component.onCompleted: { checkBluetoothStatus() }

    Connections {
        target: deviceManager
        function onBluetoothChanged() { checkBluetoothStatus() }
    }

    function loadScreen() {
        checkBluetoothStatus()

        // change screen
        appContent.state = "GatewayList"
    }

    function backAction() {
        appContent.state = "DeviceList"
    }

    function checkBluetoothStatus() {

        //console.log(">> deviceManager.bluetooth : " + deviceManager.bluetooth)
        //console.log(">> deviceManager.bluetoothAdapter : " + deviceManager.bluetoothAdapter)
        //console.log(">> deviceManager.bluetoothEnabled : " + deviceManager.bluetoothEnabled)
        //console.log(">> deviceManager.bluetoothPermissions : " + deviceManager.bluetoothPermissions)

        //console.log(">> deviceManager.permissionBluetooth : " + deviceManager.permissionBluetooth)
        //console.log(">> deviceManager.permissionLocationForeground : " + deviceManager.permissionLocationForeground)
        //console.log(">> deviceManager.permissionLocationBackground : " + deviceManager.permissionLocationBackground)
        //console.log(">> deviceManager.gpsEnabled : " + deviceManager.gpsEnabled)

        if (deviceManager.hasGateways) {
            // The device list is shown
            itemStatus.source = ""

            if (!deviceManager.bluetoothPermissions) {
                rectangleBluetoothStatus.setPermissionWarning()
            } else if (!deviceManager.bluetoothAdapter) {
                rectangleBluetoothStatus.setAdapterWarning()
            } else if (!deviceManager.bluetoothEnabled) {
                rectangleBluetoothStatus.setBluetoothWarning()
            } else {
                rectangleBluetoothStatus.hide()
            }
        } else {
            // The device list is not populated
            rectangleBluetoothStatus.hide()

            if (!deviceManager.bluetoothPermissions) {
                itemStatus.source = "components/ItemNoPermissions.qml"
            } else if (!deviceManager.bluetoothAdapter || !deviceManager.bluetoothEnabled) {
                itemStatus.source = "components/ItemNoBluetooth.qml"
            } else {
                itemStatus.source = "components/ItemNoGateway.qml"
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Column {
        id: rowbar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        z: 2

        ////////////////

        Rectangle {
            id: rectangleBluetoothStatus
            anchors.left: parent.left
            anchors.right: parent.right

            height: 0
            Behavior on height { NumberAnimation { duration: 133 } }

            clip: true
            visible: (height > 0)
            color: Theme.colorActionbar

            // prevent clicks below this area
            MouseArea { anchors.fill: parent; acceptedButtons: Qt.AllButtons; }

            Text {
                id: textBluetoothStatus
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16

                color: Theme.colorActionbarContent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                font.bold: isDesktop ? true : false
                font.pixelSize: Theme.componentFontSize
            }

            ButtonSolid {
                id: buttonBluetoothStatus
                anchors.right: parent.right
                anchors.rightMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                height: 32

                color: Theme.colorActionbarHighlight

                text: {
                    if (!deviceManager.bluetoothPermissions) return qsTr("Request")
                    if (Qt.platform.os === "android") {
                        if (!deviceManager.bluetoothEnabled) return qsTr("Enable")
                    }
                    return qsTr("Retry")
                }
                onClicked: {
                    if (!deviceManager.bluetoothPermissions) {
                        deviceManager.requestBluetoothPermissions()
                    }
                    if (!deviceManager.bluetoothEnabled || !deviceManager.bluetoothAdapter) {
                        deviceManager.enableBluetooth()
                    }
                    deviceManager.checkBluetooth()
                }
            }

            function hide() {
                rectangleBluetoothStatus.height = 0
            }
            function setAdapterWarning() {
                textBluetoothStatus.text = qsTr("Bluetooth adapter not found...")
                rectangleBluetoothStatus.height = 48
            }
            function setBluetoothWarning() {
                textBluetoothStatus.text = qsTr("Bluetooth is disabled...")
                rectangleBluetoothStatus.height = 48
            }
            function setPermissionWarning() {
                textBluetoothStatus.text = qsTr("Bluetooth permission missing...")
                rectangleBluetoothStatus.height = 48
            }
        }

        ////////////////
    }

    ////////////////////////////////////////////////////////////////////////////

    GridView {
        id: gatewaysView

        anchors.top: rowbar.bottom
        anchors.topMargin: singleColumn ? 0 : 8
        anchors.left: screenGatewayList.left
        anchors.leftMargin: 6
        anchors.right: screenGatewayList.right
        anchors.rightMargin: 6
        anchors.bottom: screenGatewayList.bottom
        anchors.bottomMargin: singleColumn ? 0 : 8

        property bool bigWidget: (!isHdpi || (isTablet && width >= 480))

        property int cellWidthTarget: {
            if (singleColumn) return gatewaysView.width
            if (isTablet) return (bigWidget ? 350 : 280)
            return (bigWidget ? 440 : 320)
        }
        property int cellColumnsTarget: Math.trunc(gatewaysView.width / cellWidthTarget)

        cellWidth: (gatewaysView.width / cellColumnsTarget)
        cellHeight: (bigWidget ? 144 : 128)

        ScrollBar.vertical: ScrollBar {
            visible: false
            anchors.right: parent.right
            anchors.rightMargin: -6
            policy: ScrollBar.AsNeeded
        }

        model: deviceManager.gatewaysList
        delegate: GatewayWidget {
            width: gatewaysView.cellWidth
            height: gatewaysView.cellHeight

            hugeMode: gatewaysView.bigWidget
            listMode: (singleColumn || gatewaysView.cellColumnsTarget === 1)
        }
    }

    ////////

    Loader {
        id: itemStatus
        anchors.fill: parent
        visible: !deviceManager.hasGateways
        asynchronous: true
    }

    ////////
}
