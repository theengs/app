import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Qt5Compat.GraphicalEffects
import "qrc:/js/UtilsPresets.js" as UtilsPresets
import ThemeEngine

Item {
    id: temperaturePreset
    anchors.fill: parent

    property var preset: null

    function loadPreset(p) {
        preset = p
    }

    function backAction() {
        //
    }

    ////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: presetHeader
        anchors.left: parent.left
        anchors.right: parent.right
        height: 96
        color: Theme.colorForeground

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: presetHeader.height
            color: Qt.darker(Theme.colorForeground, 1.03)

            IconSvg {
                anchors.centerIn: parent
                width: 40
                height: 40
                smooth: true
                color: Theme.colorSubText
                source: UtilsPresets.getPresetIcon(preset.type)
            }
        }

        TextInputThemed { // name
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: presetHeader.height
            anchors.right: parent.right
            //anchors.bottom: parent.bottom

            height: 48
            //placeholderText: qsTr("Preset name")
            fontsize: Theme.fontSizeHeader

            text: preset && preset.name
            onEditingFinished: preset.name = text
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Flickable {
        anchors.top: presetHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        contentWidth: -1
        contentHeight: columnContent.height

        topMargin: Theme.componentMargin
        bottomMargin: Theme.componentMargin

        Column {
            id: columnContent
            anchors.left: parent.left
            anchors.leftMargin: Theme.componentMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.componentMargin
            spacing: Theme.componentMargin

            ////////
/*
            Item {
                id: presetheader2
                anchors.left: parent.left
                anchors.right: parent.right
                height: 96

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 48

                    color: Theme.colorForeground

                    TextInputThemed { // name
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 48

                        //placeholderText: qsTr("Preset name")
                        text: preset && preset.name
                        onEditingFinished: preset.name = text
                    }
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 48
                    width: parent.width - 128
                    color: Qt.darker(Theme.colorForeground, 1.03)

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: Theme.componentMargin
                        anchors.rightMargin: Theme.componentMargin

                        Text {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter

                            text: qsTr("PRESET")
                            textFormat: Text.PlainText
                            color: Theme.colorSubText
                            font.pixelSize: Theme.componentFontSize
                        }

                        ButtonWireframeIcon {
                            Layout.preferredHeight: 32
                            fullColor: true
                            primaryColor: Theme.colorMaterialBlue
                            text: qsTr("SELECT TYPE")
                            source: "qrc:/assets/icons_fontawesome/question-solid.svg"
                        }
                        ButtonWireframeIcon {
                            Layout.preferredHeight: 32
                            fullColor: true
                            primaryColor: Theme.colorMaterialAmber
                            text: qsTr("DELETE")
                            source: "qrc:/assets/icons_material/baseline-delete-24px.svg"
                        }
                    }
                }

                Grid {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    rows: 2
                    columns: 2

                    Repeater {
                        model: ListModel {
                            ListElement { idx: 0; txt: ""; }
                        }
                        delegate: IconSvg {
                            source: txt
                        }
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    radius: Theme.componentRadius
                    color: "transparent"
                    border.width: 2
                    border.color: Theme.colorSeparator
                }

                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: Rectangle {
                        x: presetheader.x
                        y: presetheader.y
                        width: presetheader.width
                        height: presetheader.height
                        radius: Theme.componentRadius
                    }
                }
            }
*/
            ////////

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                height: Theme.componentHeight
                radius: Theme.componentRadius
                color: Qt.darker(Theme.colorForeground, 1.03)

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("RANGE (before)")
                    textFormat: Text.PlainText
                    color: Theme.colorSubText
                    font.pixelSize: Theme.componentFontSize
                }

                ButtonWireframeIcon {
                    anchors.right: parent.right

                    text: qsTr("ADD")
                    fullColor: true
                    primaryColor: Theme.colorMaterialLightGreen
                }
            }

            ////////

            Repeater {
                model: preset && preset.ranges

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: columnContent.height
                    radius: Theme.componentRadius
                    color: Theme.colorForeground

                    Column {
                        id: columnContent
                        anchors.left: parent.left
                        anchors.right: parent.right

                        TextInputThemed { // name
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: 40

                            text: modelData.name
                            onEditingFinished: modelData.name = text
                        }

                        RowLayout {
                            anchors.left: parent.left
                            anchors.leftMargin: Theme.componentMargin
                            anchors.right: parent.right
                            anchors.rightMargin: Theme.componentMargin
                            height: 48

                            SpinBoxThemed {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.preferredHeight: 36

                                value: modelData.tempMin
                                onValueModified: modelData.tempMin = value
                            }

                            Text {
                                Layout.alignment: Qt.AlignVCenter
                                text: ("MIN")
                                color: Theme.colorSubText
                            }

                            Item {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 36
                            }

                            Text {
                                Layout.alignment: Qt.AlignVCenter
                                text: ("MAX")
                                color: Theme.colorSubText
                            }

                            SpinBoxThemed {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.preferredHeight: 36
                                value: modelData.tempMax
                                onValueModified: modelData.tempMax = value
                            }
                        }
                    }
                }
            }

            ////////

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                height: Theme.componentHeight
                radius: Theme.componentRadius
                color: Qt.darker(Theme.colorForeground, 1.03)

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.componentMargin
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("RANGE (after)")
                    textFormat: Text.PlainText
                    color: Theme.colorSubText
                    font.pixelSize: Theme.componentFontSize
                }

                ButtonWireframeIcon {
                    anchors.right: parent.right

                    text: qsTr("ADD")
                    fullColor: true
                    primaryColor: Theme.colorMaterialLightGreen
                }
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
