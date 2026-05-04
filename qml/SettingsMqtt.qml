import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import ComponentLibrary

Item {
    id: settingsMqtt
    anchors.fill: parent

    ////////////////////////////////////////////////////////////////////////////

    // Native file picker for the MQTT TLS CA cert. On iOS this surfaces
    // UIDocumentPickerViewController (Files app + iCloud Drive); on
    // Android the Storage Access Framework; on desktop the native open
    // dialog. The selected URL is converted to a local path before
    // saving — Qt's QSslCertificate::fromPath wants a filesystem path.
    FileDialog {
        id: caFilePicker
        title: qsTr("Select MQTT TLS CA certificate")
        nameFilters: [qsTr("Certificate files (*.pem *.crt *.cer *.der)"),
                      qsTr("All files (*)")]
        fileMode: FileDialog.OpenFile

        onAccepted: {
            // selectedFile is a QUrl ("file:///abs/path"); strip the
            // scheme to a filesystem path that QSslCertificate::fromPath
            // accepts. Works on iOS (Files-app picks return file://),
            // Android SAF, and desktop native dialogs alike.
            const path = String(selectedFile).replace(/^file:\/\//, "")
            if (settingsManager.mqttTlsCaPath !== path) {
                settingsManager.mqttTlsCaPath = path
                mqttManager.reconnect_forced()
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////

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
        contentHeight: contentColumn.height

        boundsBehavior: isDesktop ? Flickable.OvershootBounds : Flickable.DragAndOvershootBounds
        ScrollBar.vertical: ScrollBar { visible: false }

        Column {
            id: contentColumn
            anchors.left: parent.left
            anchors.leftMargin: ((singleColumn || isPhone) ? 0 : parent.width * 0.12)
            anchors.right: parent.right
            anchors.rightMargin: ((singleColumn || isPhone) ? 0 : parent.width * 0.12)

            topPadding: Theme.componentMargin
            bottomPadding: Theme.componentMargin
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

                    ButtonSolid {
                        height: 28
                        anchors.verticalCenter: parent.verticalCenter

                        visible: settingsManager.mqtt
                        color: mqttManager.status ? Theme.colorGreen : Theme.colorOrange

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

                rows: 4
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
                        source: "qrc:/IconLibrary/material-symbols/storage.svg"
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
                        source: "qrc:/IconLibrary/material-symbols/pin.svg"
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
                        source: "qrc:/IconLibrary/material-icons/duotone/manage_accounts.svg"
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
                        source: "qrc:/IconLibrary/material-symbols/password.svg"
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

                rows: 2
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
                        source: "qrc:/IconLibrary/material-icons/duotone/format_size.svg"
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
                        source: "qrc:/IconLibrary/material-icons/duotone/format_size.svg"
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

                spacing: 8

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

                Row {
                    spacing: 8

                    Text {
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("Use TLS")
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.componentFontSize
                        color: Theme.colorText
                    }

                    SwitchThemed {
                        text: settingsManager.mqttTls ? qsTr("Enabled") : qsTr("Disabled")
                        checked: settingsManager.mqttTls
                        onClicked: {
                            settingsManager.mqttTls = checked
                            mqttManager.reconnect_forced()
                        }
                    }
                }

                Row {
                    visible: settingsManager.mqttTls
                    width: parent.width
                    spacing: 8

                    TextFieldThemed {
                        id: tf_mqtt_tls_ca
                        width: parent.width - browseCaButton.width - parent.spacing
                        height: 36

                        selectByMouse: true
                        placeholderText: qsTr("Custom CA cert path (optional)")
                        text: settingsManager.mqttTlsCaPath

                        onEditingFinished: {
                            if (settingsManager.mqttTlsCaPath !== text) {
                                settingsManager.mqttTlsCaPath = text
                                mqttManager.reconnect_forced()
                            }
                        }

                        IconSvg {
                            width: 20; height: 20;
                            anchors.right: parent.right
                            anchors.rightMargin: 12
                            anchors.verticalCenter: parent.verticalCenter

                            color: Theme.colorSubText
                            source: "qrc:/IconLibrary/material-symbols/lock.svg"
                        }
                    }

                    ButtonSolid {
                        id: browseCaButton
                        height: 36

                        text: qsTr("Browse")
                        onClicked: caFilePicker.open()
                    }
                }

                Row {
                    visible: settingsManager.mqttTls
                    spacing: 8

                    Text {
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("Skip certificate validation")
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.componentFontSize
                        color: Theme.colorText
                    }

                    SwitchThemed {
                        text: settingsManager.mqttTlsInsecure ? qsTr("Enabled") : qsTr("Disabled")
                        checked: settingsManager.mqttTlsInsecure
                        onClicked: {
                            settingsManager.mqttTlsInsecure = checked
                            mqttManager.reconnect_forced()
                        }
                    }
                }

                Text {
                    visible: settingsManager.mqttTls && settingsManager.mqttTlsInsecure
                    width: parent.width

                    text: qsTr("Disables MITM protection — use only with trusted private brokers.")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    color: Theme.colorRed
                    font.pixelSize: Theme.fontSizeContentSmall
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

    ////////////////////////////////////////////////////////////////////////////
}
