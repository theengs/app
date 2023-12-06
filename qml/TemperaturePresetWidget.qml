import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "qrc:/js/UtilsPresets.js" as UtilsPresets
import ThemeEngine

SwipeDelegate {
    id: temperaturePresetWidget

    implicitWidth: 256
    implicitHeight: 80

    clip: true
    padding: Theme.componentMargin

    ////////////////////////////////////////////////////////////////////////////

    Loader {
        id: popupDeleteLoader

        active: false
        asynchronous: false

        sourceComponent: PopupPresetDelete {
            id: popupDelete
            onConfirmed: presetsManager.removePreset(modelData.name)
            onClosed: swipe.close()
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    background: Rectangle {
        color: temperaturePresetWidget.pressed ? Qt.darker(Theme.colorLowContrast, 1.05) : Theme.colorLowContrast

        Rectangle {
            anchors.right: parent.right
            anchors.rightMargin: -12
            anchors.verticalCenter: parent.verticalCenter

            width: parent.height*0.33
            height: parent.height*1.33
            rotation: 10
            antialiasing: true
            color: parent.color
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 1
            color: Theme.colorSeparator
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    contentItem: Item {
        RowLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: Theme.componentMargin

            ////////

            Item {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 48
                Layout.alignment: Qt.AlignVCenter

                IconSvg {
                    anchors.centerIn: parent
                    width: 36
                    height: 36
                    smooth: true
                    color: Theme.colorSubText
                    source: UtilsPresets.getPresetIcon(modelData.type)
                }
            }

            ////////

            Column {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                spacing: Theme.componentMargin / 3

                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    text: modelData.name
                    textFormat: Text.PlainText
                    //font.capitalization: Text.Capitalize
                    font.pixelSize: Theme.fontSizeContentVeryBig
                    color: Theme.colorText
                    elide: Text.ElideRight
                }

                ////

                Row { // preset count & min/max
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: Theme.componentMargin

                    Text {
                        visible: (modelData.rangeCount === 0)
                        text: qsTr("No temperature range defined")
                        font.pixelSize: Theme.fontSizeContent
                        color: Theme.colorSubText
                    }

                    Text { // count
                        visible: modelData.rangeCount
                        text: qsTr("%1 range(s)").arg(modelData.rangeCount)
                        font.pixelSize: Theme.fontSizeContent
                        color: Theme.colorSubText
                    }

                    Text { // min/max
                        function unitCelsiusToFahrenheitOrNot(temp_unit) {
                            if (temp_unit === 0) return "°C"
                            return "°F"
                        }
                        function tempCelsiusToFahrenheitOrNot(temp_c, temp_unit) {
                            if (temp_unit === 0) return temp_c.toFixed(0)
                            return (temp_c * 1.8 + 32).toFixed(1);
                        }

                        visible: modelData.rangeCount
                        text: {
                            if (modelData.rangeMax > 0) {
                                return qsTr("(%1°%0 to %2°%0)")
                                    .arg(settingsManager.tempUnit)
                                    .arg(tempCelsiusToFahrenheitOrNot(modelData.rangeMin, settingsManager.appUnits))
                                    .arg(tempCelsiusToFahrenheitOrNot(modelData.rangeMax, settingsManager.appUnits))
                            } else {
                                return qsTr("(%1°%0 and up)")
                                    .arg(settingsManager.tempUnit)
                                    .arg(tempCelsiusToFahrenheitOrNot(modelData.rangeMin, settingsManager.appUnits))
                            }
                        }
                        font.pixelSize: Theme.fontSizeContent
                        color: Theme.colorSubText
                    }
                }

                ////
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    swipe.enabled: !modelData.readOnly
    swipe.right: Row {
        anchors.right: parent.right
        height: parent.height

        Item {
            id: deleteLabel
            width: parent.height*1.33
            height: parent.height

            Rectangle {
                anchors.fill: parent
                color: temperaturePresetWidget.background.color
            }

            Rectangle {
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: 10

                width: parent.height*1.33
                height: parent.height*2
                rotation: 10
                antialiasing: true

                color: deleteLabel.SwipeDelegate.pressed ? Qt.darker(Theme.colorMaterialOrange, 1.1) : Theme.colorMaterialOrange
                border.width: 2
                border.color: Qt.darker(color, 1.1)
            }

            Column {
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: 8

                IconSvg {
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "qrc:/assets/icons_material/baseline-delete-24px.svg"
                    color: "white"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Supprimer")
                    font.bold: true
                    color: "white"
                }
            }

            SwipeDelegate.onClicked: {
                utilsApp.vibrate(33)
                popupDeleteLoader.active = true
                popupDeleteLoader.item.open()
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
