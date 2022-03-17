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
        ScrollBar.vertical: ScrollBar { visible: isDesktop }

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

                IconSvg {
                    id: image_appsettings
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: screenPaddingLeft + 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/logos/mqtt.svg"
                }

                Text {
                    id: text_appsettings
                    anchors.left: image_appsettings.right
                    anchors.leftMargin: 24
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

            ////////

            Text {
                id: legend_database
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 12

                text: qsTr("Connects to a remote MySQL compatible database, instead of the embedded database. Allows multiple instances of the application to share data. Database setup is at your own charge.")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
                verticalAlignment: Text.AlignBottom
            }

            ////////

            Loader {
                anchors.left: parent.left
                anchors.right: parent.right

                asynchronous: true
                Component.onCompleted: {
                    sourceComponent = mqttSettingsScalable
                }
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Component {
        id: mqttSettingsScalable

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

                placeholderText: qsTr("Host")
                text: settingsManager.mqttHost
                onEditingFinished: settingsManager.mqttHost = text
                selectByMouse: true

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

                placeholderText: qsTr("Port")
                text: settingsManager.mqttPort
                onEditingFinished: settingsManager.mqttPort = parseInt(text, 10)
                validator: IntValidator { bottom: 1; top: 65535; }
                selectByMouse: true

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

                placeholderText: qsTr("User")
                text: settingsManager.mqttUser
                onEditingFinished: settingsManager.mqttUser = text
                selectByMouse: true

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

                placeholderText: qsTr("Password")
                text: settingsManager.mqttPassword
                onEditingFinished: settingsManager.mqttPassword = text
                selectByMouse: true
                echoMode: TextInput.PasswordEchoOnEdit

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
    }

    ////////
}
