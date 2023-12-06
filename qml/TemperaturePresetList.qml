import QtQuick
import QtQuick.Controls

import ThemeEngine

Loader {
    id: screenTemperaturePresetList
    anchors.fill: parent

    ////////////////////////////////////////////////////////////////////////////

    function loadScreen() {
        // load screen
        screenTemperaturePresetList.active = true

        // change screen
        appContent.state = "TemperaturePresetList"
    }

    function backAction() {
        if (screenTemperaturePresetList.sourceComponent) {
            screenTemperaturePresetList.item.backAction()
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    active: false
    asynchronous: true

    sourceComponent: Item {
        anchors.fill: parent
        anchors.margins: 0

        function backAction() {
            if (stackView.depth > 1) {
                stackView.pop()
                return
            }

            screenDeviceList.loadScreen()
        }

        ////////////////

        StackView {
            id: stackView
            anchors.fill: parent

            initialItem: mainView
        }

        ////////////////

        Component {
            id: mainView

            Item {
                anchors.fill: undefined

                ListView {
                    anchors.fill: parent

                    topMargin: Theme.componentMargin
                    bottomMargin: Theme.componentMargin
                    spacing: 0

                    section.property: "readOnly"
                    section.delegate: Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 48

                        color: Theme.colorForeground

                        required property bool section

                        IconSvg {
                            id: image_appsettings
                            width: 24
                            height: 24
                            anchors.left: parent.left
                            anchors.leftMargin: screenPaddingLeft + 16
                            anchors.verticalCenter: parent.verticalCenter

                            color: Theme.colorIcon
                            source: "qrc:/assets/icons_material/baseline-settings-20px.svg"
                        }

                        Text {
                            id: text_appsettings
                            anchors.left: image_appsettings.right
                            anchors.leftMargin: 24
                            anchors.verticalCenter: parent.verticalCenter

                            text: section ? qsTr("Application presets") : qsTr("User presets")
                            textFormat: Text.PlainText
                            font.pixelSize: Theme.fontSizeContent
                            font.bold: false
                            color: Theme.colorText
                            wrapMode: Text.WordWrap
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    model: presetsManager.presets
                    delegate: TemperaturePresetWidget {
                        width: ListView.view.width
                        height: 96

                        onClicked: {
                            stackView.push(detailsView)
                            stackView.get(1).loadPreset(modelData)
                        }
                    }
                }

                PopupPresetAdd {
                    id: popupPresetAdd
                }

                RoundButtonIconShadow { // add
                    anchors.right: parent.right
                    anchors.rightMargin: 24
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: (Qt.platform.os === "android" && screenOrientation === Qt.PortraitOrientation) ? 12 : 24

                    width: 56
                    height: 56

                    source: "qrc:/assets/icons_material/baseline-add-24px.svg"
                    iconColor: "white"
                    background: true
                    backgroundColor: Theme.colorPrimary

                    onClicked: {
                        popupPresetAdd.open()
                    }
                }
            }
        }

        ////////////////

        Component {
            id: detailsView

            TemperaturePreset {
                anchors.fill: undefined
            }
        }

        ////////////////
    }

    ////////////////////////////////////////////////////////////////////////////
}
