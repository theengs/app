import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "qrc:/js/UtilsPresets.js" as UtilsPresets
import ComponentLibrary

SwipeDelegate {
    id: batteryPresetSearchWidget

    implicitWidth: 256
    implicitHeight: 64

    clip: true
    padding: Theme.componentMargin

    property bool selected: false

    ////////////////////////////////////////////////////////////////////////////

    background: Rectangle {
        color: batteryPresetSearchWidget.pressed ?
                   Qt.darker(Theme.colorLowContrast, 1.05) :
                   Theme.colorLowContrast

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
            anchors.leftMargin: 8
            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.verticalCenter: parent.verticalCenter
            spacing: Theme.componentMargin

            ////////

            Item {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 48
                Layout.alignment: Qt.AlignVCenter

                IconSvg {
                    anchors.centerIn: parent
                    width: 32
                    height: 32
                    smooth: true
                    color: Theme.colorSubText
                    source: UtilsPresets.getBatteryPresetIcon(modelData.type)
                }
            }

            ////////

            Column {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                spacing: 0

                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    text: modelData.name
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContentBig
                    color: Theme.colorText
                    elide: Text.ElideRight
                }

                ////

                Row {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: 4

                    Text {
                        text: modelData.voltageMin.toFixed(1) + "v"
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.fontSizeContentSmall
                        color: Theme.colorText
                    }

                    Text {
                        text: "to"
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.fontSizeContentSmall
                        color: Theme.colorText
                    }

                    Text {
                        text: modelData.voltageMax.toFixed(1) + "v"
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.fontSizeContentSmall
                        color: Theme.colorText
                    }
                }

                ////
            }

            ////////

            ButtonClear {
                visible: batteryPresetSearchWidget.selected

                color: Theme.colorGreen
                source: "qrc:/IconLibrary/material-symbols/check.svg"
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
