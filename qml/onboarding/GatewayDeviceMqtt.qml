import QtQuick
import QtQuick.Controls

import ComponentLibrary
import DeviceUtils

Item {
    id: gatewayMqtt
    anchors.fill: parent

    ////////////////////////////////////////////////////////////////////////////

    function loadSubScreen() {
        console.log("deviceGateway // loadSubScreen(mqtt) >> " + currentGateway)

        deviceGateway.state = "mqtt"
    }

    function backAction() {
        console.log("gatewayMqtt.backAction()")
/*
        if (tf_mqtt_host.focus) {
            tf_mqtt_host.focus = false
            return
        }
        if (tf_mqtt_port.focus) {
            tf_mqtt_port.focus = false
            return
        }
        if (tf_mqtt_user.focus) {
            tf_mqtt_user.focus = false
            return
        }
        if (tf_mqtt_pwd.focus) {
            tf_mqtt_pwd.focus = false
            return
        }
        if (tf_mqtt_topicA.focus) {
            tf_mqtt_topicA.focus = false
            return
        }
        if (tf_mqtt_topicB.focus) {
            tf_mqtt_topicB.focus = false
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
