import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "qrc:/js/UtilsPresets.js" as UtilsPresets
import ThemeEngine

SwipeDelegate {
    id: temperaturePresetSearchWidget

    implicitWidth: 256
    implicitHeight: 64

    clip: true
    padding: Theme.componentMargin

    ////////////////////////////////////////////////////////////////////////////

    background: Rectangle {
        color: temperaturePresetSearchWidget.pressed ?
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
                    source: UtilsPresets.getPresetIcon(modelData.type)
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
                    //font.capitalization: Text.Capitalize
                    font.pixelSize: Theme.fontSizeContentBig
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
                        font.pixelSize: Theme.fontSizeContentSmall
                        color: Theme.colorSubText
                    }

                    Text { // count
                        visible: modelData.rangeCount
                        text: qsTr("%1 range(s)").arg(modelData.rangeCount)
                        font.pixelSize: Theme.fontSizeContentSmall
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
                            if (modelData.tempMaxEnabled) {
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
                        font.pixelSize: Theme.fontSizeContentSmall
                        color: Theme.colorSubText
                    }
                }

                ////
            }

            ////////
        }
/*
        ItemTag {
            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.verticalCenter: parent.verticalCenter

            visible: modelData.readOnly
            text: qsTr("app preset")
            color: Qt.darker(Theme.colorForeground, 1.04)
        }
*/
    }

    ////////////////////////////////////////////////////////////////////////////
}
