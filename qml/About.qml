import QtQuick
import QtQuick.Controls

import ComponentLibrary

Item {
    id: aboutScreen
    anchors.fill: parent

    ////////////////////////////////////////////////////////////////////////////

    Flickable {
        anchors.fill: parent

        contentWidth: parent.width
        contentHeight: contentColumn.height

        boundsBehavior: isDesktop ? Flickable.OvershootBounds : Flickable.DragAndOvershootBounds
        ScrollBar.vertical: ScrollBar { visible: false }

        Column {
            id: contentColumn
            anchors.left: parent.left
            anchors.leftMargin: ((singleColumn || isPhone) ? 0 : parent.width * 0.12)
            anchors.right: parent.right
            anchors.rightMargin: ((singleColumn || isPhone) ? 0 : parent.width * 0.12)

            ////////////////

            Rectangle { // header area
                anchors.left: parent.left
                anchors.right: parent.right

                height: 92
                color: headerUnicolor ? Theme.colorBackground : Theme.colorForeground

                Row {
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    z: 2
                    height: 92
                    spacing: 24

                    Image { // logo
                        width: 64
                        height: 64
                        anchors.verticalCenter: parent.verticalCenter
                        source: "qrc:/assets/logos/logo.svg"
                    }

                    Column {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: 2
                        spacing: 0

                        Text {
                            text: "Theengs"
                            color: Theme.colorText
                            font.pixelSize: 28
                        }
                        Text {
                            color: Theme.colorSubText
                            text: qsTr("version %1 %2").arg(utilsApp.appVersion()).arg(utilsApp.appBuildMode())
                            font.pixelSize: Theme.fontSizeContentBig
                        }
                    }
                }

                Row {
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    visible: wideWideMode
                    spacing: 16

                    ButtonSolid {
                        width: 160

                        text: qsTr("WEBSITE")
                        source: "qrc:/IconLibrary/material-symbols/link.svg"
                        sourceSize: 28
                        color: (Theme.currentTheme === Theme.THEME_NIGHT) ? Theme.colorHeader : "#5483EF"

                        onClicked: Qt.openUrlExternally("https://app.theengs.io")
                    }

                    ButtonSolid {
                        width: 160

                        text: qsTr("SUPPORT")
                        source: "qrc:/IconLibrary/material-symbols/support.svg"
                        sourceSize: 22
                        color: (Theme.currentTheme === Theme.THEME_NIGHT) ? Theme.colorHeader : "#5483EF"

                        onClicked: Qt.openUrlExternally("https://github.com/theengs/app/issues/new")
                    }
                }
            }

            ////////////////

            Item { height: 8; width: 8; visible: !wideWideMode; } // spacer

            Row {
                id: buttonsRow
                height: 56

                anchors.left: parent.left
                anchors.leftMargin: leftPadding + 16
                anchors.right: parent.right
                anchors.rightMargin: rightPadding + 16

                visible: !wideWideMode
                spacing: 16

                ButtonSolid {
                    width: ((parent.width - 16) / 2)
                    anchors.verticalCenter: parent.verticalCenter


                    text: qsTr("WEBSITE")
                    source: "qrc:/IconLibrary/material-symbols/link.svg"
                    sourceSize: 28
                    color: (Theme.currentTheme === Theme.THEME_NIGHT) ? Theme.colorHeader : "#5483EF"

                    onClicked: Qt.openUrlExternally("https://app.theengs.io")
                }
                ButtonSolid {
                    width: ((parent.width - 16) / 2)
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("SUPPORT")
                    source: "qrc:/IconLibrary/material-symbols/support.svg"
                    sourceSize: 22
                    color: (Theme.currentTheme === Theme.THEME_NIGHT) ? Theme.colorHeader : "#5483EF"

                    onClicked: Qt.openUrlExternally("https://github.com/theengs/app/issues/new")
                }
            }

            ////////////////

            Item { height: 4; width: 4; visible: isDesktop; } // spacer

            ListItem { // description
                width: parent.width
                text: qsTr("Manufacturer agnostic BLE sensors reading application with MQTT integration.")
                source: "qrc:/IconLibrary/material-symbols/info.svg"
                sourceSize: 24
            }

            Item { height: 4; width: 4; } // spacer

            ////////

            ListSeparator { }

            ListItemClickable { // release notes
                width: parent.width

                text: qsTr("Theengs App")
                source: "qrc:/IconLibrary/material-symbols/new_releases.svg"
                sourceSize: 24
                indicatorSource: "qrc:/IconLibrary/material-icons/duotone/launch.svg"

                onClicked: Qt.openUrlExternally("https://github.com/theengs/app/releases")

                ItemBadge {
                    anchors.right: parent.right
                    anchors.rightMargin: 48
                    anchors.verticalCenter: parent.verticalCenter

                    text: "1.5"
                }
            }

            ////////

            ListSeparator { }

            ListItemClickable { // release notes
                width: parent.width

                text: qsTr("Theengs Decoder")
                source: "qrc:/IconLibrary/material-symbols/new_releases.svg"
                sourceSize: 24
                indicatorSource: "qrc:/IconLibrary/material-icons/duotone/launch.svg"

                onClicked: Qt.openUrlExternally("https://github.com/theengs/decoder/releases/tag/v2.0.0")

                ItemBadge {
                    anchors.right: parent.right
                    anchors.rightMargin: 48
                    anchors.verticalCenter: parent.verticalCenter

                    text: "2.0.0"
                }
            }

            ////////

            ListSeparator { visible: (Qt.platform.os === "android" || Qt.platform.os === "ios") }

            ListItemClickable { // permissions
                width: parent.width
                visible: (Qt.platform.os === "android" || Qt.platform.os === "ios")

                text: qsTr("About app permissions")
                source: "qrc:/IconLibrary/material-symbols/flaky.svg"
                sourceSize: 24
                indicatorSource: "qrc:/IconLibrary/material-symbols/chevron_right.svg"

                onClicked: screenAboutPermissions.loadScreenFrom("About")
            }

            ////////

            ListSeparator { }

            ////////

            Item { // list dependencies
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft + Theme.componentMargin
                anchors.right: parent.right
                anchors.rightMargin: screenPaddingRight + Theme.componentMargin

                height: 40 + dependenciesText.height + dependenciesColumn.height

                IconSvg {
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: 4
                    anchors.verticalCenter: dependenciesText.verticalCenter

                    source: "qrc:/IconLibrary/material-symbols/settings.svg"
                    color: Theme.colorSubText
                }

                Text {
                    id: dependenciesText
                    anchors.top: parent.top
                    anchors.topMargin: 16
                    anchors.left: parent.left
                    anchors.leftMargin: appHeader.headerPosition - parent.anchors.leftMargin
                    anchors.right: parent.right
                    anchors.rightMargin: 8

                    text: qsTr("This application is made possible thanks to a couple of third party open source projects:")
                    textFormat: Text.PlainText
                    color: Theme.colorSubText
                    font.pixelSize: Theme.fontSizeContent
                    wrapMode: Text.WordWrap
                }

                Column {
                    id: dependenciesColumn
                    anchors.top: dependenciesText.bottom
                    anchors.topMargin: 8
                    anchors.left: parent.left
                    anchors.leftMargin: appHeader.headerPosition - parent.anchors.leftMargin
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    spacing: 4

                    Repeater {
                        model: [
                            "Theengs decoder (LGPL v3)",
                            "Qt6 (LGPL v3)",
                            "QtMqtt (GPL v3)",
                            "Mbed TLS (Apache 2.0)",
                            "MobileUI (MIT)",
                            "MobileSharing (MIT)",
                            "SingleApplication (MIT)",
                            "Google Material Icons (Apache 2.0)",
                        ]
                        delegate: Text {
                            width: parent.width
                            text: "- " + modelData
                            textFormat: Text.PlainText
                            color: Theme.colorSubText
                            font.pixelSize: Theme.fontSizeContent
                            wrapMode: Text.WordWrap
                        }
                    }
                }
            }

            ListSeparator { }

            ////////

            Loader { // list debug info
                active: utilsApp.isDebugBuild()
                asynchronous: true
                sourceComponent: Item {
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.componentMargin
                    height: 32 + debugColumn.height

                    IconSvg {
                        anchors.top: debugColumn.top
                        anchors.topMargin: 0
                        anchors.left: parent.left
                        anchors.leftMargin: 4
                        width: 24
                        height: 24

                        source: "qrc:/IconLibrary/material-symbols/info.svg"
                        color: Theme.colorSubText
                    }

                    MouseArea {
                        anchors.fill: parent
                        onPressAndHold: {
                            var txt = ""
                            txt += "App name: %1".arg(utilsApp.appName()) + "\n"
                            txt += "App version: %1".arg(utilsApp.appVersion()) + "\n"
                            txt += "Build mode: %1".arg(utilsApp.appBuildModeFull()) + "\n"
                            txt += "Build architecture: %1".arg(utilsApp.qtArchitecture()) + "\n"
                            txt += "Build date: %1".arg(utilsApp.appBuildDateTime()) + "\n"
                            txt += "Qt version: %1".arg(utilsApp.qtVersion()) + "\n"
                            utilsClipboard.setText(txt)
                        }
                    }

                    Column {
                        id: debugColumn
                        anchors.left: parent.left
                        anchors.leftMargin: appHeader.headerPosition - parent.anchors.leftMargin
                        anchors.right: parent.right
                        anchors.rightMargin: 8
                        anchors.verticalCenter: parent.verticalCenter

                        spacing: Theme.componentMargin * 0.33

                        Text {
                            color: Theme.colorSubText
                            text: "App name: %1".arg(utilsApp.appName())
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContent
                        }
                        Text {
                            color: Theme.colorSubText
                            text: "App version: %1".arg(utilsApp.appVersion())
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContent
                        }
                        Text {
                            color: Theme.colorSubText
                            text: "Build mode: %1".arg(utilsApp.appBuildModeFull())
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContent
                        }
                        Text {
                            color: Theme.colorSubText
                            text: "Build architecture: %1".arg(utilsApp.qtArchitecture())
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContent
                        }
                        Text {
                            color: Theme.colorSubText
                            text: "Build date: %1".arg(utilsApp.appBuildDateTime())
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContent
                        }
                        Text {
                            color: Theme.colorSubText
                            text: "Qt version: %1".arg(utilsApp.qtVersion())
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContent
                        }
                    }
                }
            }

            ListSeparator { visible: utilsApp.isDebugBuild() }

            ////////
        }
    }
}
