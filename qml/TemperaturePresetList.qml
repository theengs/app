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
        appContent.state = "TemperatureRanges"
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

        Component {
            id: mainView

            ListView {
                model: presetsManager.presets
                delegate: TemperaturePresetWidget {
                    width: parent.width
                    height: 96

                    onClicked: {
                        stackView.push(detailsView)
                        stackView.get(1).loadPreset(modelData)
                    }
                }
            }
        }

        Component {
            id: detailsView

            TemperaturePreset {
                anchors.fill: undefined
            }
        }

        ////////////////

        PopupPresetAdd {
            id: popupPresetAdd
        }

        RoundButtonIconShadow {
            id: add

            anchors.right: parent.right
            anchors.rightMargin: 24
            anchors.bottom: parent.bottom
            anchors.bottomMargin: (Qt.platform.os === "android" && screenOrientation === Qt.PortraitOrientation) ? 12 : 24

            width: 56
            height: 56
            visible: (stackView.depth === 1)

            source: "qrc:/assets/icons_material/baseline-add-24px.svg"
            iconColor: "white"
            background: true
            backgroundColor: Theme.colorPrimary

            onClicked: {
                popupPresetAdd.open()
            }
        }

        ////////////////
    }

    ////////////////////////////////////////////////////////////////////////////
}
