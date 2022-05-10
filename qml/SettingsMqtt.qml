import QtQuick 2.15
import QtQuick.Controls 2.15

import ThemeEngine 1.0
import "qrc:/js/UtilsNumber.js" as UtilsNumber

Item {
    id: mqttScreen
    width: 480
    height: 720
    anchors.fill: parent

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
            spacing: 16

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
/*
                    Text {
                        anchors.verticalCenter: parent.verticalCenter

                        text: settingsManager.mqtt ? qsTr("enabled") : qsTr("disabled")
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.fontSizeContent
                        color: Theme.colorText
                    }

                    SwitchThemedMobile {
                        anchors.verticalCenter: parent.verticalCenter

                        checked: settingsManager.mqtt
                        onClicked: {
                            settingsManager.mqtt = checked

                            if (checked) mqttManager.connect()
                            else  mqttManager.disconnect()
                        }
                    }
*/
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

                text: qsTr("Connects to a remote MQTT broker. Setup is at your own charge.")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
                verticalAlignment: Text.AlignBottom
            }

            ////////

            Text {
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

                SwitchThemedDesktop {
                    text: settingsManager.mqtt ? qsTr("Enabled") : qsTr("Disabled")
                    checked: settingsManager.mqtt
                    onClicked: {
                        settingsManager.mqtt = checked

                        if (checked) mqttManager.connect()
                        else  mqttManager.disconnect()
                    }
                }
            }

            ////////

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 16

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
                        settingsManager.mqttHost = text
                        mqttManager.reconnect_forced()
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
                        settingsManager.mqttPort = parseInt(text, 10)
                        mqttManager.reconnect_forced()
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
                        settingsManager.mqttUser = text
                        mqttManager.reconnect_forced()
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
                        settingsManager.mqttPassword = text
                        mqttManager.reconnect_forced()
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

                rows: 4
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
                        settingsManager.mqttTopicA = text
                        mqttManager.reconnect_forced()
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
                        settingsManager.mqttTopicB = text
                        mqttManager.reconnect_forced()
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

            Column {
                anchors.right: parent.right
                anchors.rightMargin: 16
                spacing: 16

                ButtonWireframe {
                    text: qsTr("Save")
                    fullColor: true

                    onClicked: {
                        focus = true
                        if (settingsManager.mqtt) mqttManager.reconnect_forced()
                    }
                }
/*
                ButtonWireframe {
                    text: mqttManager.status ? qsTr("Connected") : qsTr("Disconnected")
                    primaryColor: mqttManager.status ? Theme.colorGreen : Theme.colorOrange
                    fullColor: true
                }
*/
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
        }
    }

    ////////
}
