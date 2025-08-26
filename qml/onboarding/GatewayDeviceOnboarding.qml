import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import ComponentLibrary
import DeviceUtils

Item {
    id: gatewayOnboarding
    anchors.fill: parent

    ////////////////////////////////

    function loadSubScreen() {
        console.log("deviceGateway // loadSubScreen(onboarding) >> " + currentGateway)

        deviceGateway.state = "onboarding"
        reset()
    }

    function backAction() {
        console.log("gatewayOnboarding.backAction()")

        currentGateway.actionDisconnect()

        // TODO // clear textfields

        appContent.state = "GatewayDevice"
        deviceGateway.state = ""
    }

    function reset() {
        // reset SwipeView
        swipeview.disableAnimation()
        swipeview.currentIndex = 0
        swipeview.enableAnimation()

        // reset gateway screen
        gateway.resetGateway()

        // reset WiFi screen
        networks.resetNetwork()

        // reset MQTT screen
        brokers.resetBroker()
    }

    ////////////////////////////////

    SwipeView {
        id: swipeview

        anchors.fill: parent
        anchors.leftMargin: screenPaddingLeft
        anchors.rightMargin: screenPaddingRight

        interactive: false

        currentIndex: 0
        onCurrentIndexChanged: {
            if (currentIndex < 0) currentIndex = 0
            if (currentIndex > count-1) {
                //currentIndex = 0 // reset
            }
        }

        function enableAnimation() {
            contentItem.highlightMoveDuration = 333
        }
        function disableAnimation() {
            contentItem.highlightMoveDuration = 0
        }

        property int contentSz: singleColumn ? (swipeview.width - 2*24) :
                                               Math.min(((swipeview.width - 2*24) / 2), 512) - 4

        ////////////////////////////////////////////////////////////////////////

        Item {
            id: page1

            Column {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.right: parent.right
                anchors.rightMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                spacing: 32

                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter

                    text: qsTr("Your gateway is plugged.")
                    wrapMode: Text.WordWrap
                    color: Theme.colorSubText
                    font.pixelSize: Theme.fontSizeContentBig
                }

                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter

                    text: qsTr("Connect your smartphone to a 2.4 GHz WiFi network.")
                    wrapMode: Text.WordWrap
                    color: Theme.colorSubText
                    font.pixelSize: Theme.fontSizeContentBig
                }

                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter

                    text: qsTr("Look for a steady orange light.")+ "<br>" +
                          qsTr("If absent, hold the gateway button for 20 seconds.")
                    wrapMode: Text.WordWrap
                    color: Theme.colorSubText
                    font.pixelSize: Theme.fontSizeContentBig
                }

                ButtonSolid {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Start onboarding!")
                    onClicked: swipeview.currentIndex++
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////

        Item {
            id: page2

            // gateway name & password

            Column {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.right: parent.right
                anchors.rightMargin: 24

                topPadding: 16
                bottomPadding: page2_bottom.height + 64
                spacing: 24

                ////

                Column {
                    id: gateway
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: 8

                    function resetGateway() {
                        tf_gateway_name.clear()
                        tf_gateway_pwd.clear()
                    }

                    Text {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: Theme.componentHeight

                        text: qsTr("Gateway:")
                        textFormat: Text.PlainText
                        wrapMode: Text.WordWrap
                        color: Theme.colorSubText
                        font.pixelSize: Theme.fontSizeContentBig
                        verticalAlignment: Text.AlignBottom
                    }

                    Text {
                        width: swipeview.contentSz
                        horizontalAlignment: Text.AlignHCenter

                        text: "Demo text Demo text Demo text Demo text Demo text Demo text Demo text Demo text Demo text ."
                        wrapMode: Text.WordWrap
                        color: Theme.colorSubText
                        font.pixelSize: Theme.fontSizeContentBig
                    }

                    TextFieldThemed {
                        id: tf_gateway_name
                        width: swipeview.contentSz
                        height: 36

                        selectByMouse: true
                        maximumLength: 64

                        placeholderText: qsTr("Name")

                        onEditingFinished: {
                            //
                        }

                        IconSvg {
                            width: 20; height: 20;
                            anchors.right: parent.right
                            anchors.rightMargin: 12
                            anchors.verticalCenter: parent.verticalCenter

                            color: Theme.colorSubText
                            source: "qrc:/IconLibrary/material-icons/duotone/manage_accounts.svg"
                        }
                    }

                    TextFieldThemed {
                        id: tf_gateway_pwd
                        width: swipeview.contentSz
                        height: 36

                        selectByMouse: true
                        echoMode: TextInput.Password
                        maximumLength: 64

                        placeholderText: qsTr("Password")

                        onEditingFinished: {
                            //
                        }

                        IconSvg {
                            width: 20; height: 20;
                            anchors.right: parent.right
                            anchors.rightMargin: 12
                            anchors.verticalCenter: parent.verticalCenter

                            color: Theme.colorSubText
                            source: "qrc:/IconLibrary/material-symbols/password.svg"
                        }
                    }
                }

                ////
            }
/*
            Flow {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.right: parent.right
                anchors.rightMargin: 24
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 24

                spacing: Theme.componentMargin

                property int btnCount: 1
                property int btnSize: singleColumn ? width : ((width-spacing*btnCount) / (btnCount+1))

                ButtonSolid {
                    width: parent.btnSize

                    text: qsTr("Save settings")
                    color: Theme.colorOrange

                    onClicked: {
                        swipeview.currentIndex++
                    }
                }
            }
*/
            ////////

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                height: page2_bottom.height + 32
                color: Theme.colorForeground

                Column {
                    id: page2_bottom
                    anchors.top: parent.top
                    anchors.topMargin: 16
                    anchors.left: parent.left
                    anchors.leftMargin: 24
                    anchors.right: parent.right
                    anchors.rightMargin: 24
                    spacing: 12

                    Row {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 24

                        Repeater {
                            model: 3
                            Rectangle {
                                width : (parent.width - 24*2) / 3
                                height: 16
                                radius: 4
                                color: (index === swipeview.currentIndex-1) ? Theme.colorGreen : Theme.colorMaterialDarkGrey
                                border.width: (index === swipeview.currentIndex-1) ? 2 : 0
                                border.color: Qt.darker(color, 1.1)
                            }
                        }
                    }

                    RowLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: Theme.componentMargin

                        ButtonSolid {
                            //text: qsTr("Back")
                            source: "qrc:/IconLibrary/material-symbols/arrow_left.svg"
                            color: Theme.colorGrey

                            onClicked: {
                                swipeview.currentIndex--
                            }
                        }

                        ButtonSolid {
                            Layout.fillWidth: true

                            text: qsTr("Save settings")
                            color: Theme.colorOrange

                            onClicked: {
                                //tf_gateway_name.text
                                //tf_gateway_pwd.text

                                currentGateway.setGatewayCredentials(tf_gateway_pwd.text)
                                currentGateway.actionPasswordSet()

                                swipeview.currentIndex++
                            }
                        }
                    }
                }
            }

            ////////
        }

        ////////////////////////////////////////////////////////////////////////

        Item {
            id: page3

            Flickable {
                anchors.fill: parent

                contentWidth: parent.width
                contentHeight: columnWifi.height

                boundsBehavior: isDesktop ? Flickable.OvershootBounds : Flickable.DragAndOvershootBounds
                ScrollBar.vertical: ScrollBar { visible: false }

                Column {
                    id: columnWifi
                    anchors.left: parent.left
                    anchors.leftMargin: 24
                    anchors.right: parent.right
                    anchors.rightMargin: 24

                    topPadding: 16
                    bottomPadding: page3_bottom.height + 64
                    spacing: 24

                    ////

                    Column {
                        id: networks
                        anchors.left: parent.left
                        anchors.right: parent.right

                        property string selected: ""

                        function selectNetwork(name) {
                            if (networks.selected === name) {
                                networks.resetNetwork()
                            } else {
                                networks.selected = name
                                tf_wifi_ssid.text = name
                                tf_wifi_pwd.clear()
                            }
                        }
                        function resetNetwork() {
                            networks.selected = ""
                            tf_wifi_ssid.clear()
                            tf_wifi_pwd.clear()
                            cb_wifi_showpwd.checked = false
                        }

                        RowLayout {
                            id: wifiWarning
                            anchors.left: parent.left
                            anchors.right: parent.right

                            height: Theme.componentHeight
                            spacing: Theme.componentMargin
                            visible: (isMobile && utilsWifi.currentSSID.length === 0)

                            Timer {
                                interval: 1000
                                running: (isMobile && appContent.state === "GatewayDevice" && deviceGateway.state === "onboarding")
                                repeat: true
                                onTriggered: {
                                    utilsWifi.refreshWiFi()
                                    //wifiWarning.visible = (utilsWifi.currentSSID.length === 0)
                                }
                            }

                            IconSvg {
                                Layout.preferredWidth: 20
                                Layout.preferredHeight: 20
                                source: "qrc:/IconLibrary/material-symbols/warning.svg"
                                color: Theme.colorWarning
                            }

                            //Text { text: utilsWifi.currentSSID + " " +  utilsWifi.currentSSID.length }

                            Text {
                                Layout.fillWidth: true

                                text: qsTr("Your device is not connected to a WiFi network...")
                                textFormat: Text.PlainText
                                wrapMode: Text.WordWrap
                                color: Theme.colorSubText
                                font.pixelSize: Theme.fontSizeContentBig
                                verticalAlignment: Text.AlignBottom
                            }
                        }

                        Text {
                            height: Theme.componentHeight
                            anchors.left: parent.left
                            anchors.right: parent.right

                            text: qsTr("Networks available:")
                            textFormat: Text.PlainText
                            wrapMode: Text.WordWrap
                            color: Theme.colorSubText
                            font.pixelSize: Theme.fontSizeContentBig
                            verticalAlignment: Text.AlignBottom
                        }

                        Connections {
                            target: selectedGateway
                            function onNetworksUpdated() {
                                utilsWifi.refreshWiFi()
                            }
                        }

                        Repeater {
                            model: selectedGateway.networksAvailable
                            //onCountChanged: utilsWifi.refreshWiFi()

                            ItemNetwork {
                                width: parent.width

                                name: modelData.ssid
                                open: modelData.open
                                connected: (modelData.ssid === utilsWifi.currentSSID)
                                strength: modelData.strength

                                selected: (networks.selected === name)
                                onClicked: networks.selectNetwork(name)
                            }
                        }
                        ItemNetworkInfo {
                            width: parent.width

                            empty: (selectedGateway.networksAvailable.length === 0)
                            scanning: currentGateway.networksRefreshing
                            visible: (empty || scanning)
                        }

                        ButtonClear {
                            anchors.right: parent.right
                            anchors.rightMargin: 0

                            visible: currentGateway.connected && !currentGateway.networksRefreshing

                            animation: "rotate"
                            animationRunning: currentGateway.networksRefreshing
                            color: Theme.colorBlue
                            source: "qrc:/IconLibrary/material-symbols/autorenew.svg"

                            onClicked: {
                                if (deviceManager.bluetooth && currentGateway)
                                    currentGateway.getWifiNetworks()
                            }
                        }
                    }

                    ////

                    Column {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 8

                        Text {
                            height: Theme.componentHeight
                            anchors.left: parent.left
                            anchors.right: parent.right

                            text: qsTr("My network:")
                            textFormat: Text.PlainText
                            wrapMode: Text.WordWrap
                            color: Theme.colorSubText
                            font.pixelSize: Theme.fontSizeContentBig
                            verticalAlignment: Text.AlignBottom
                        }

                        TextFieldThemed {
                            id: tf_wifi_ssid
                            width: swipeview.contentSz
                            height: 36

                            selectByMouse: true
                            placeholderText: qsTr("SSID")
                            maximumLength: 32

                            onEditingFinished: {
                                //
                            }

                            IconSvg {
                                width: 20; height: 20;
                                anchors.right: parent.right
                                anchors.rightMargin: 12
                                anchors.verticalCenter: parent.verticalCenter

                                color: Theme.colorSubText
                                source: "qrc:/IconLibrary/material-symbols/wifi.svg"
                            }
                        }

                        TextFieldThemed {
                            id: tf_wifi_pwd
                            width: swipeview.contentSz
                            height: 36

                            selectByMouse: true
                            placeholderText: qsTr("Password")

                            maximumLength: 64
                            echoMode: cb_wifi_showpwd.checked ? TextInput.Normal : TextInput.Password

                            onEditingFinished: {
                                //
                            }

                            IconSvg {
                                width: 20; height: 20;
                                anchors.right: parent.right
                                anchors.rightMargin: 12
                                anchors.verticalCenter: parent.verticalCenter

                                color: Theme.colorSubText
                                source: "qrc:/IconLibrary/material-symbols/key.svg"
                            }
                        }

                        Item {
                            width: swipeview.contentSz
                            height: 36

                            CheckBoxThemed {
                                id: cb_wifi_showpwd
                                height: 36
                                anchors.right: parent.right
                                layoutDirection: Qt.RightToLeft
                                text: qsTr("Show password")
                            }
                        }
                    }

                    ////
                }
            }

            ////////

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                height: page3_bottom.height + 32
                color: Theme.colorForeground

                Column {
                    id: page3_bottom
                    anchors.top: parent.top
                    anchors.topMargin: 16
                    anchors.left: parent.left
                    anchors.leftMargin: 24
                    anchors.right: parent.right
                    anchors.rightMargin: 24
                    spacing: 12

                    Row {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 24

                        Repeater {
                            model: 3
                            Rectangle {
                                width : (parent.width - 24*2) / 3
                                height: 16
                                radius: 4
                                color: (index === swipeview.currentIndex-1) ? Theme.colorGreen : Theme.colorMaterialDarkGrey
                                border.width: (index === swipeview.currentIndex-1) ? 2 : 0
                                border.color: Qt.darker(color, 1.1)
                            }
                        }
                    }
/*
                    ButtonSolid {
                        width: parent.width

                        text: qsTr("Skip WiFi setup")
                        color: Theme.colorGrey

                        onClicked: {
                            networks.resetNetwork()
                            swipeview.currentIndex++
                        }
                    }
*/
                    RowLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: Theme.componentMargin

                        ButtonSolid {
                            //text: qsTr("Back")
                            source: "qrc:/IconLibrary/material-symbols/arrow_left.svg"
                            color: Theme.colorGrey

                            onClicked: {
                                swipeview.currentIndex--
                            }
                        }

                        ButtonSolid {
                            text: qsTr("Skip WiFi setup")
                            color: Theme.colorGrey

                            onClicked: {
                                networks.resetNetwork()
                                swipeview.currentIndex++
                            }
                        }

                        ButtonSolid {
                            Layout.fillWidth: true

                            text: qsTr("Save settings")
                            color: Theme.colorOrange

                            onClicked: {
                                //tf_wifi_ssid.text
                                //tf_wifi_pwd.text

                                currentGateway.setWifiCredentials(tf_wifi_ssid.text, tf_wifi_pwd.text)
                                currentGateway.setWifiCreds()
                                currentGateway.setWifiConn()

                                swipeview.currentIndex++
                            }
                        }
                    }
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////

        Item {
            id: page4

            Flickable {
                anchors.fill: parent

                contentWidth: parent.width
                contentHeight: columnMqtt.height

                boundsBehavior: isDesktop ? Flickable.OvershootBounds : Flickable.DragAndOvershootBounds
                ScrollBar.vertical: ScrollBar { visible: false }

                Column {
                    id: columnMqtt
                    anchors.left: parent.left
                    anchors.leftMargin: 24
                    anchors.right: parent.right
                    anchors.rightMargin: 24

                    topPadding: 16
                    bottomPadding: page4_bottom.height + 64
                    spacing: 24

                    ////

                    Column {
                        id: brokers
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 4

                        property string selected: ""

                        function selectBroker(name) {
                            if (brokers.selected === name) {
                                resetBroker()
                            } else {
                                brokers.selected = name
                            }
                        }
                        function resetBroker() {
                            brokers.selected = ""
                            tf_mqtt_host.clear()
                            tf_mqtt_port.clear()
                            tf_mqtt_user.clear()
                            tf_mqtt_pwd.clear()
                            cb_mqtt_showpwd.checked = false
                            cb_mqtt_secure.checked = false
                            tf_gateway_name.clear()
                            tf_gateway_pwd.clear()
                            tf_mqtt_topicA.clear()
                            tf_mqtt_topicB.clear()
                        }

                        Text {
                            height: Theme.componentHeight
                            anchors.left: parent.left
                            anchors.right: parent.right

                            text: qsTr("Brokers available:")
                            textFormat: Text.PlainText
                            wrapMode: Text.WordWrap
                            color: Theme.colorSubText
                            font.pixelSize: Theme.fontSizeContentBig
                            verticalAlignment: Text.AlignBottom
                        }

                        Repeater {
                            model: mqttManager.brokersAvailable

                            ItemBroker {
                                width: parent.width

                                name: modelData.name
                                host: modelData.host
                                port: modelData.port

                                selected: (brokers.selected === name)
                                onClicked: {
                                    if (brokers.selected === name) {
                                        brokers.resetBroker()
                                    } else {
                                        brokers.selected = name
                                        tf_mqtt_host.text = modelData.host
                                        tf_mqtt_port.text = modelData.port
                                        tf_mqtt_user.text = modelData.username
                                        tf_mqtt_pwd.text = modelData.password
                                        tf_mqtt_topicA.text = modelData.topicA
                                        tf_mqtt_topicB.text = modelData.topicB
                                    }
                                }
                            }
                        }
                    }

                    ////

                    Column {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 8

                        Text {
                            height: Theme.componentHeight
                            anchors.left: parent.left
                            anchors.right: parent.right

                            text: qsTr("My broker:")
                            textFormat: Text.PlainText
                            wrapMode: Text.WordWrap
                            color: Theme.colorSubText
                            font.pixelSize: Theme.fontSizeContentBig
                            verticalAlignment: Text.AlignBottom
                        }

                        Grid {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            rows: 6
                            columns: singleColumn ? 1 : 2
                            spacing: 12

                            TextFieldThemed {
                                id: tf_mqtt_host
                                width: swipeview.contentSz
                                height: 36

                                selectByMouse: true
                                maximumLength: 64

                                placeholderText: qsTr("Host")
                                text: settingsManager.mqttHost

                                onEditingFinished: {
                                    //
                                }

                                IconSvg {
                                    width: 20; height: 20;
                                    anchors.right: parent.right
                                    anchors.rightMargin: 12
                                    anchors.verticalCenter: parent.verticalCenter

                                    color: Theme.colorSubText
                                    source: "qrc:/IconLibrary/material-symbols/storage.svg"
                                }
                            }

                            TextFieldThemed {
                                id: tf_mqtt_port
                                width: swipeview.contentSz
                                height: 36

                                selectByMouse: true
                                maximumLength: 5
                                validator: IntValidator { bottom: 1; top: 65535; }

                                placeholderText: qsTr("Port")
                                text: settingsManager.mqttPort

                                onEditingFinished: {
                                    //
                                }

                                IconSvg {
                                    width: 20; height: 20;
                                    anchors.right: parent.right
                                    anchors.rightMargin: 12
                                    anchors.verticalCenter: parent.verticalCenter

                                    color: Theme.colorSubText
                                    source: "qrc:/IconLibrary/material-symbols/pin.svg"
                                }
                            }

                            TextFieldThemed {
                                id: tf_mqtt_user
                                width: swipeview.contentSz
                                height: 36

                                selectByMouse: true
                                maximumLength: 64

                                placeholderText: qsTr("User")
                                text: settingsManager.mqttUser

                                onEditingFinished: {
                                    //
                                }

                                IconSvg {
                                    width: 20; height: 20;
                                    anchors.right: parent.right
                                    anchors.rightMargin: 12
                                    anchors.verticalCenter: parent.verticalCenter

                                    color: Theme.colorSubText
                                    source: "qrc:/IconLibrary/material-icons/duotone/manage_accounts.svg"
                                }
                            }

                            TextFieldThemed {
                                id: tf_mqtt_pwd
                                width: swipeview.contentSz
                                height: 36

                                selectByMouse: true
                                maximumLength: 64
                                echoMode: cb_mqtt_showpwd.checked ? TextInput.Normal : TextInput.Password

                                placeholderText: qsTr("Password")
                                text: settingsManager.mqttPassword

                                onEditingFinished: {
                                    //
                                }

                                IconSvg {
                                    width: 20; height: 20;
                                    anchors.right: parent.right
                                    anchors.rightMargin: 12
                                    anchors.verticalCenter: parent.verticalCenter

                                    color: Theme.colorSubText
                                    source: "qrc:/IconLibrary/material-symbols/password.svg"
                                }
                            }

                            Item {
                                width: swipeview.contentSz * (singleColumn ? 1 : 2)
                                height: 36

                                CheckBoxThemed {
                                    id: cb_mqtt_showpwd
                                    height: 36
                                    anchors.right: parent.right
                                    layoutDirection: Qt.RightToLeft
                                    text: qsTr("Show password")
                                }
                            }

                            Item {
                                width: swipeview.contentSz * (singleColumn ? 1 : 2)
                                height: 36

                                SwitchThemed {
                                    id: cb_mqtt_secure
                                    height: 36
                                    text: qsTr("MQTT secure connection")
                                }
                            }
                        }
                    }

                    ////

                    Column {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 8

                        // home
                        // topic : 2e > mettre adresse mac de la gateway

                        Text {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: Theme.componentHeight

                            text: qsTr("Topics:")
                            textFormat: Text.PlainText
                            wrapMode: Text.WordWrap
                            color: Theme.colorSubText
                            font.pixelSize: Theme.fontSizeContentBig
                            verticalAlignment: Text.AlignBottom
                        }

                        Grid {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            rows: 2
                            columns: singleColumn ? 1 : 2
                            spacing: 12

                            TextFieldThemed {
                                id: tf_mqtt_topicA
                                width: swipeview.contentSz
                                height: 36

                                selectByMouse: true
                                placeholderText: qsTr("Base topic")
                                text: settingsManager.mqttTopicA

                                onEditingFinished: {
                                    //
                                }

                                IconSvg {
                                    width: 20; height: 20;
                                    anchors.right: parent.right
                                    anchors.rightMargin: 12
                                    anchors.verticalCenter: parent.verticalCenter

                                    color: Theme.colorSubText
                                    source: "qrc:/IconLibrary/material-icons/duotone/format_size.svg"
                                }
                            }

                            TextFieldThemed {
                                id: tf_mqtt_topicB
                                width: swipeview.contentSz
                                height: 36

                                selectByMouse: true
                                placeholderText: qsTr("Discovery prefix")
                                text: settingsManager.mqttTopicB

                                onEditingFinished: {
                                    //
                                }

                                IconSvg {
                                    width: 20; height: 20;
                                    anchors.right: parent.right
                                    anchors.rightMargin: 12
                                    anchors.verticalCenter: parent.verticalCenter

                                    color: Theme.colorSubText
                                    source: "qrc:/IconLibrary/material-icons/duotone/format_size.svg"
                                }
                            }
                        }
                    }

                    ////
                }
            }

            ////////

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                height: page4_bottom.height + 32
                color: Theme.colorForeground

                Column {
                    id: page4_bottom
                    anchors.top: parent.top
                    anchors.topMargin: 16
                    anchors.left: parent.left
                    anchors.leftMargin: 24
                    anchors.right: parent.right
                    anchors.rightMargin: 24
                    spacing: 12

                    Row {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 24

                        Repeater {
                            model: 3
                            Rectangle {
                                width : (parent.width - 24*2) / 3
                                height: 16
                                radius: 4
                                color: (index === swipeview.currentIndex-1) ? Theme.colorGreen : Theme.colorMaterialDarkGrey
                                border.width: (index === swipeview.currentIndex-1) ? 2 : 0
                                border.color: Qt.darker(color, 1.1)
                            }
                        }
                    }

                    RowLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: Theme.componentMargin

                        ButtonSolid {
                            //text: qsTr("Back")
                            source: "qrc:/IconLibrary/material-symbols/arrow_left.svg"
                            color: Theme.colorGrey

                            onClicked: {
                                swipeview.currentIndex--
                            }
                        }

                        ButtonSolid {
                            Layout.fillWidth: true

                            text: qsTr("Save settings")
                            color: Theme.colorOrange

                            onClicked: {
                                //tf_mqtt_host.text
                                //tf_mqtt_port.text
                                //tf_mqtt_user.text
                                //tf_mqtt_pwd.text
                                //tf_mqtt_topicA.text
                                //tf_mqtt_topicB.text

                                currentGateway.setMqttCredentials(tf_mqtt_host.text, tf_mqtt_port.text,
                                                                  tf_mqtt_user.text, tf_mqtt_pwd.text,
                                                                  tf_mqtt_topicA.text, tf_mqtt_topicB.text)
                                currentGateway.actionMqttSet()

                                swipeview.currentIndex++
                            }
                        }
                    }
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////

        Item {
            id: page5

            function reset() { }

            Column {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.right: parent.right
                anchors.rightMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                spacing: 32

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter

                    text: qsTr("Your gateway has been successfully setup!")
                    wrapMode: Text.WordWrap
                    color: Theme.colorSubText
                    font.pixelSize: Theme.fontSizeContentBig
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter

                    text: qsTr("You can now use it.")
                    wrapMode: Text.WordWrap
                    color: Theme.colorSubText
                    font.pixelSize: Theme.fontSizeContentBig
                }

                ButtonSolid {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Finish")
                    onClicked: {
                        gatewayOnboarding.reset()
                        gatewayOnboarding.backAction()
                    }
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////
    }

    ////////////////////////////////
}
