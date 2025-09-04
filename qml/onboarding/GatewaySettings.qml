import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import ComponentLibrary
import DeviceUtils

Item {
    id: gatewaySettings
    anchors.fill: parent

    ////////////////////////////////////////////////////////////////////////////

    function loadSubScreen() {
        console.log("deviceGateway // loadSubScreen(settings) >> " + currentGateway)

        deviceGateway.state = "settings"
    }

    function backAction() {
        console.log("gatewaySettings.backAction()")
/*
        if (tf_wifi_ssid.focus) {
            tf_wifi_ssid.focus = false
            return
        }
*/
        appContent.state = "GatewayDevice"
        deviceGateway.state = ""
    }

    ////////////////////////////////////////////////////////////////////////////

    Column {
        id: contentColumn
        anchors.left: parent.left
        anchors.leftMargin: 16
        anchors.right: parent.right
        anchors.rightMargin: 16

        topPadding: Theme.componentMarginXL
        bottomPadding: Theme.componentMarginXL
        spacing: Theme.componentMargin

        property int sz: singleColumn ? contentColumn.width : Math.min((contentColumn.width / 2), 512) - 4

        ////////

        // TODO

        ////////

        Item  { width: 4; height: 4; }

        ButtonSolid {
            width: parent.width
            text: qsTr("Save")

            onClicked: {
                //
            }
        }

        ////////
    }

    ////////////////////////////////////////////////////////////////////////////
}
