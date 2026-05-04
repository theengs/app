import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import ComponentLibrary

Item {
    id: settingsMqtt
    anchors.fill: parent

    // Whether the always-visible "MITM disabled" warning text under the
    // Skip-validation toggle is expanded. Default: shown when insecure
    // is on (so the user sees it the first time they enable). User can
    // collapse via the (?) info icon next to the toggle.
    property bool tlsWarningExpanded: settingsManager.mqttTlsInsecure

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

                // Hint when TLS is on but the user is still pointing at the
                // plaintext default port — frequent foot-gun for HiveMQ
                // Cloud / mosquitto-tls users.
                Text {
                    visible: settingsManager.mqttTls && settingsManager.mqttPort === 1883
                    width: parent.width

                    text: qsTr("TLS brokers typically listen on port 8883.")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    color: Theme.colorSubText
                    font.pixelSize: Theme.fontSizeContentSmall
                }

                // TLS sub-options: visually grouped under "Use TLS" via a
                // left rule so they read as children, not peers, of the
                // parent toggle.
                Row {
                    visible: settingsManager.mqttTls
                    width: parent.width
                    spacing: 12

                    Rectangle {
                        width: 2
                        height: tlsSubColumn.height
                        color: Theme.colorSeparator
                    }

                    Column {
                        id: tlsSubColumn
                        width: parent.width - 14   // 2 (rule) + 12 (spacing)
                        spacing: 6

                        Text {
                            text: qsTr("Custom CA certificate")
                            textFormat: Text.PlainText
                            color: Theme.colorSubText
                            font.pixelSize: Theme.fontSizeContent
                        }

                        TextFieldThemed {
                            id: tf_mqtt_tls_ca
                            width: parent.width
                            height: 36

                            selectByMouse: true
                            placeholderText: qsTr("Path (tap folder icon to browse)")
                            text: settingsManager.mqttTlsCaPath

                            onEditingFinished: {
                                if (settingsManager.mqttTlsCaPath !== text) {
                                    settingsManager.mqttTlsCaPath = text
                                    mqttManager.reconnect_forced()
                                }
                            }

                            // Tappable folder icon inside the field opens
                            // the native file picker. Replaces the
                            // separate Browse button and consolidates the
                            // pattern with the other broker fields' icons.
                            Item {
                                width: 36; height: 36
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter

                                IconSvg {
                                    anchors.centerIn: parent
                                    width: 22; height: 22
                                    color: caFolderArea.pressed ? Theme.colorPrimary : Theme.colorSubText
                                    source: "qrc:/IconLibrary/material-symbols/folder_open.svg"
                                }

                                MouseArea {
                                    id: caFolderArea
                                    anchors.fill: parent
                                    onClicked: caFilePicker.open()
                                }
                            }
                        }

                        // Skip-validation toggle + (?) info icon +
                        // collapsible warning. (?) toggles the warning
                        // visibility so it doesn't permanently eat 2 lines
                        // of vertical space after the user has acked it.
                        Row {
                            spacing: 8

                            Text {
                                anchors.verticalCenter: parent.verticalCenter

                                text: qsTr("Skip validation")
                                textFormat: Text.PlainText
                                font.pixelSize: Theme.componentFontSize
                                color: Theme.colorText
                            }

                            SwitchThemed {
                                text: settingsManager.mqttTlsInsecure ? qsTr("Enabled") : qsTr("Disabled")
                                checked: settingsManager.mqttTlsInsecure
                                onClicked: {
                                    settingsManager.mqttTlsInsecure = checked
                                    if (checked) settingsMqtt.tlsWarningExpanded = true
                                    mqttManager.reconnect_forced()
                                }
                            }

                            Item {
                                visible: settingsManager.mqttTlsInsecure
                                width: 28; height: 28
                                anchors.verticalCenter: parent.verticalCenter

                                IconSvg {
                                    anchors.centerIn: parent
                                    width: 18; height: 18
                                    color: settingsMqtt.tlsWarningExpanded ? Theme.colorRed : Theme.colorSubText
                                    source: "qrc:/IconLibrary/material-symbols/info.svg"
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: settingsMqtt.tlsWarningExpanded = !settingsMqtt.tlsWarningExpanded
                                }
                            }
                        }

                        Text {
                            visible: settingsManager.mqttTlsInsecure && settingsMqtt.tlsWarningExpanded
                            width: parent.width

                            text: qsTr("Disables MITM protection — use only with trusted private brokers.")
                            textFormat: Text.PlainText
                            wrapMode: Text.WordWrap
                            color: Theme.colorRed
                            font.pixelSize: Theme.fontSizeContentSmall
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

                text: qsTr("Activity")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentBig
                verticalAlignment: Text.AlignBottom
            }

            ////////

            // Bounded log viewer. Cap at ~200px; older entries scroll into
            // view without pushing the rest of the form off-screen.
            // Stays mounted even when empty so the user keeps a visual
            // anchor after tapping Clear (and so future entries
            // re-populate in place rather than reflowing the form).
            // Lines are color-coded by severity (parsed from the message
            // text the C++ side prepends).
            Rectangle {
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 16

                height: 200
                color: Theme.colorComponentBackground
                radius: 6
                border.width: 1
                border.color: Theme.colorSeparator

                Text {
                    anchors.centerIn: parent
                    visible: mqttManager.log.length === 0

                    text: qsTr("No activity yet — events will appear here.")
                    textFormat: Text.PlainText
                    color: Theme.colorSubText
                    font.pixelSize: Theme.fontSizeContentSmall
                    font.italic: true
                }

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 8
                    clip: true
                    visible: mqttManager.log.length > 0

                    Column {
                        width: parent.width
                        spacing: 2

                        Repeater {
                            // Newest is at the start of mqttManager.log
                            // (prepend order); the C++ cap keeps this at
                            // ≤100 lines so split() is cheap.
                            model: mqttManager.log.split("\n").filter(function(l) { return l.length > 0 })

                            delegate: Text {
                                width: parent.width
                                text: modelData
                                textFormat: Text.PlainText
                                wrapMode: Text.WrapAnywhere
                                font.family: "Menlo"
                                font.pixelSize: Theme.fontSizeContentSmall
                                color: {
                                    if (modelData.indexOf("error") >= 0) return Theme.colorRed
                                    if (modelData.indexOf("disconnected") >= 0) return Theme.colorWarning
                                    if (modelData.indexOf("connecting") >= 0) return Theme.colorSubText
                                    if (modelData.indexOf("connected") >= 0) return Theme.colorGreen
                                    return Theme.colorText
                                }
                            }
                        }
                    }
                }
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
