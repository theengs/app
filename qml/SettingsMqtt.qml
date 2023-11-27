import QtQuick 2.15
import QtQuick.Controls 2.15

import ThemeEngine 1.0
import "qrc:/js/UtilsNumber.js" as UtilsNumber

Item {
    id: mqttScreen
    width: 480
    height: 720
    anchors.fill: parent

    function backAction() {
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

        appContent.state = "DeviceList"
    }

    ////////////////////////////////////////////////////////////////////////////

    Flickable {
        anchors.fill: parent

        contentWidth: -1
        contentHeight: column.height

        boundsBehavior: isDesktop ? Flickable.OvershootBounds : Flickable.DragAndOvershootBounds
        ScrollBar.vertical: ScrollBar { visible: false }

        Column {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right

            topPadding: 12
            bottomPadding: 12
            spacing: 12

            ////////////////

            Rectangle {
                height: 48
                anchors.left: parent.left
                anchors.right: parent.right

                color: Theme.colorForeground

                Row {
                    anchors.left: parent.left
                    anchors.leftMargin: screenPaddingLeft + 16
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 24

                    IconSvg {
                        id: image_appsettings
                        width: 24
                        height: 24
                        anchors.verticalCenter: parent.verticalCenter

                        color: Theme.colorIcon
                        source: "qrc:/assets/logos/mqtt.svg"
                    }

                    Text {
                        id: text_appsettings
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("MQTT")
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.fontSizeContent
                        font.bold: false
                        color: Theme.colorText
                        wrapMode: Text.WordWrap
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Row {
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 4

                    ButtonWireframe {
                        height: 28
                        anchors.verticalCenter: parent.verticalCenter

                        visible: settingsManager.mqtt
                        fullColor: true
                        primaryColor: mqttManager.status ? Theme.colorGreen : Theme.colorOrange

                        text: mqttManager.status ? qsTr("Connected") : qsTr("Disconnected")
                    }
                }
            }

            ////////

            Text {
                id: legend_database
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 12
                height: 24

                text: qsTr("Connects to a remote MQTT broker.")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
                verticalAlignment: Text.AlignBottom
            }

            ////////

            Text {
                height: Theme.componentHeight
                anchors.left: parent.left
                anchors.leftMargin: 16

                text: qsTr("Status")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentBig
                verticalAlignment: Text.AlignBottom
            }

            ////////

            Column {
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 16

                Row {
                    spacing: 8

                    Text {
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("MQTT")
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.componentFontSize
                        color: Theme.colorText
                    }

                    SwitchThemed {
                        text: settingsManager.mqtt ? qsTr("Enabled") : qsTr("Disabled")
                        checked: settingsManager.mqtt
                        onClicked: {
                            settingsManager.mqtt = checked

                            if (checked) mqttManager.connect()
                            else mqttManager.disconnect()
                        }
                    }
                }
            }

            ////////

            Text {
                height: Theme.componentHeight
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 16

                text: qsTr("Broker")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentBig
                verticalAlignment: Text.AlignBottom
            }

            ////////

            Grid {
                id: grid
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 16

                rows: singleColumn ? 4 : 2
                columns: singleColumn ? 1 : 2
                spacing: 12

                property int sz: singleColumn ? grid.width : Math.min((grid.width / 2), 512) - 4

                TextFieldThemed {
                    id: tf_mqtt_host
                    width: grid.sz
                    height: 36

                    selectByMouse: true
                    placeholderText: qsTr("Host")
                    text: settingsManager.mqttHost

                    onEditingFinished: {
                        if (settingsManager.mqttHost !== text) {
                            settingsManager.mqttHost = text
                            mqttManager.reconnect_forced()
                        }
                    }

                    IconSvg {
                        width: 20; height: 20;
                        anchors.right: parent.right
                        anchors.rightMargin: 12
                        anchors.verticalCenter: parent.verticalCenter

                        color: Theme.colorSubText
                        source: "qrc:/assets/icons_material/baseline-storage-24px.svg"
                    }
                }

                TextFieldThemed {
                    id: tf_mqtt_port
                    width: grid.sz
                    height: 36

                    selectByMouse: true
                    placeholderText: qsTr("Port")
                    text: settingsManager.mqttPort
                    validator: IntValidator { bottom: 1; top: 65535; }

                    onEditingFinished: {
                        if (settingsManager.mqttPort !== parseInt(text, 10)) {
                            settingsManager.mqttPort = parseInt(text, 10)
                            mqttManager.reconnect_forced()
                        }
                    }

                    IconSvg {
                        width: 20; height: 20;
                        anchors.right: parent.right
                        anchors.rightMargin: 12
                        anchors.verticalCenter: parent.verticalCenter

                        color: Theme.colorSubText
                        source: "qrc:/assets/icons_material/baseline-pin-24px.svg"
                    }
                }

                TextFieldThemed {
                    id: tf_mqtt_user
                    width: grid.sz
                    height: 36

                    selectByMouse: true
                    placeholderText: qsTr("User")
                    text: settingsManager.mqttUser

                    onEditingFinished: {
                        if (settingsManager.mqttUser !== text) {
                            settingsManager.mqttUser = text
                            mqttManager.reconnect_forced()
                        }
                    }

                    IconSvg {
                        width: 20; height: 20;
                        anchors.right: parent.right
                        anchors.rightMargin: 12
                        anchors.verticalCenter: parent.verticalCenter

                        color: Theme.colorSubText
                        source: "qrc:/assets/icons_material/duotone-manage_accounts-24px.svg"
                    }
                }

                TextFieldThemed {
                    id: tf_mqtt_pwd
                    width: grid.sz
                    height: 36

                    selectByMouse: true
                    placeholderText: qsTr("Password")
                    text: settingsManager.mqttPassword
                    echoMode: TextInput.PasswordEchoOnEdit

                    onEditingFinished: {
                        if (settingsManager.mqttPassword !== text) {
                            settingsManager.mqttPassword = text
                            mqttManager.reconnect_forced()
                        }
                    }

                    IconSvg {
                        width: 20; height: 20;
                        anchors.right: parent.right
                        anchors.rightMargin: 12
                        anchors.verticalCenter: parent.verticalCenter

                        color: Theme.colorSubText
                        source: "qrc:/assets/icons_material/baseline-password-24px.svg"
                    }
                }
            }

            ////////

            Text {
                height: Theme.componentHeight
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 12

                text: qsTr("Topics")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentBig
                verticalAlignment: Text.AlignBottom
            }

            ////////

            Grid {
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 16

                rows: singleColumn ? 2 : 1
                columns: singleColumn ? 1 : 2
                spacing: 12

                TextFieldThemed {
                    id: tf_mqtt_topicA
                    width: grid.sz
                    height: 36

                    selectByMouse: true
                    placeholderText: qsTr("Topic A")
                    text: settingsManager.mqttTopicA

                    onEditingFinished: {
                        if (settingsManager.mqttTopicA !== text) {
                            settingsManager.mqttTopicA = text
                            mqttManager.reconnect_forced()
                        }
                    }

                    IconSvg {
                        width: 20; height: 20;
                        anchors.right: parent.right
                        anchors.rightMargin: 12
                        anchors.verticalCenter: parent.verticalCenter

                        color: Theme.colorSubText
                        source: "qrc:/assets/icons_material/duotone-format_size-24px.svg"
                    }
                }

                TextFieldThemed {
                    id: tf_mqtt_topicB
                    width: grid.sz
                    height: 36

                    selectByMouse: true
                    placeholderText: qsTr("Topic B")
                    text: settingsManager.mqttTopicB

                    onEditingFinished: {
                        if (settingsManager.mqttTopicB !== text) {
                            settingsManager.mqttTopicB = text
                            mqttManager.reconnect_forced()
                        }
                    }

                    IconSvg {
                        width: 20; height: 20;
                        anchors.right: parent.right
                        anchors.rightMargin: 12
                        anchors.verticalCenter: parent.verticalCenter

                        color: Theme.colorSubText
                        source: "qrc:/assets/icons_material/duotone-format_size-24px.svg"
                    }
                }
            }

            ////////

            Text {
                height: Theme.componentHeight
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 12

                text: qsTr("Options")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentBig
                verticalAlignment: Text.AlignBottom
            }

            ////////

            Column {
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 16

                Row {
                    spacing: 8

                    Text {
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("Discovery")
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.componentFontSize
                        color: Theme.colorText
                    }

                    SwitchThemed {
                        text: settingsManager.mqttDiscovery ? qsTr("Enabled") : qsTr("Disabled")
                        checked: settingsManager.mqttDiscovery
                        onClicked: settingsManager.mqttDiscovery = checked
                    }
                }
            }

            ////////

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 16

                text: mqttManager.log
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
            }

            ////////
        }
    }

    ////////
}
